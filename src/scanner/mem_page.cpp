#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <fstream>      // std::ifstream

#include "process_info.hpp"
#include "mem_page.hpp"
#include "utils.hpp"

#define READ_PROCESS_MEMORY_SZ        (1024*4)

mem_page::mem_page(Process_Info *process_info, MEMORY_BASIC_INFORMATION *p_mem_info)
  : m_proc_info(process_info),
    m_buffer(new uint8_t[p_mem_info->RegionSize]())
{
    memcpy(&m_mem_info, p_mem_info, sizeof(MEMORY_BASIC_INFORMATION));
}

mem_page::mem_page(PVOID BaseAddress, std::string file_path)
  : m_proc_info(nullptr)
{
  std::ifstream file(file_path.c_str(), std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  m_mem_info.BaseAddress = BaseAddress;
  m_mem_info.RegionSize = size;

  //std::vector<char> buffer(size);
  //if (file.read(buffer.data(), size))

  m_buffer = std::unique_ptr<uint8_t>(new uint8_t[m_mem_info.RegionSize]());
  if (file.read((char*)m_buffer.get(), size))
  {
  }
}

mem_page::~mem_page()
{
  if (m_buffer.get() != nullptr)
    m_buffer.release();
}

int mem_page::copy_page()
{
  uint32_t remote_start_addr = reinterpret_cast<uint32_t>(m_mem_info.BaseAddress);

  // make sure initialize was already called
  assert(m_buffer != nullptr);
  assert(m_buffer.get() != nullptr);

  for (uint32_t bytes_read = 0; bytes_read < m_mem_info.RegionSize; )
  {
    PCVOID remote_byte_addr = reinterpret_cast<PCVOID>(remote_start_addr + bytes_read);   // remote address of bytes to read
    uint32_t current_bytes_to_read = m_mem_info.RegionSize - bytes_read;                            // number remote bytes that will be copied on this read

    // limit read size if remaining bytes > maximum bytes that can be copied in one read
    if (current_bytes_to_read > READ_PROCESS_MEMORY_SZ)
      current_bytes_to_read = READ_PROCESS_MEMORY_SZ;

    if (ReadProcessMemory(m_proc_info->get_process_handle(), remote_byte_addr, this->get_buffer(bytes_read), current_bytes_to_read, NULL) == 0)
    {
      std::cerr << "Fail to read process memory at byte " << reinterpret_cast<uint32_t>(remote_byte_addr) << std::endl;
      debug_print_last_win_error();
      return -1;
    }

    bytes_read += current_bytes_to_read;
  }

  return 0;
}

int mem_page::initialize()
{
  if (m_buffer.get() != nullptr)
    m_buffer.release();

  // allocate space
  m_buffer = std::unique_ptr<uint8_t>(new uint8_t[m_mem_info.RegionSize]());

  return 0;
}

int mem_page::refresh_page()
{
  MEMORY_BASIC_INFORMATION temp_mem_info;

  // reread memory information for this block
  if (VirtualQueryEx(m_proc_info->get_process_handle(), this->get_base_addr(), &temp_mem_info, sizeof(temp_mem_info)) == 0)
  {
    std::cerr << "Unable to query address: 0x" << CONFIGURE_HEX << (uint32_t)this->get_base_addr() << CONFIGURE_DEC << std::endl;
    return -1;
  }

  // ignore uncommitted / non-writable / guarded pages
  if (!(temp_mem_info.State & MEM_COMMIT)
    ||  !(temp_mem_info.Protect & MEMINFO_PROTECT_IS_WRITABLE)
    ||  (temp_mem_info.Protect & PAGE_GUARD))
  {
    std::cerr << "Unable to read address: 0x" << CONFIGURE_HEX << (uint32_t)this->get_base_addr() << CONFIGURE_DEC << std::endl;
    return -1;
  }

  // buffer length doesn't match value returned from virtualquery
  if (m_mem_info.RegionSize != temp_mem_info.RegionSize)
  {
    std::cerr << "Allocated length did not match for base address: 0x" << CONFIGURE_HEX << (uint32_t)this->get_base_addr() << CONFIGURE_DEC << std::endl;
    return -1;
  }

  // std::cout << "Copied " << m_len << " bytes from base address 0x" << CONFIGURE_HEX << (uint32_t)m_base_addr << CONFIGURE_DEC << std::endl;
  return this->copy_page();

  // return 0;
}

PVOID mem_page::get_base_addr()
{
  return m_mem_info.BaseAddress;
}

uint32_t mem_page::get_len()
{
  return m_mem_info.RegionSize;
}

//void mem_page::print()
//{
//  // print the entire page
//  return print(get_len());
//}

//void mem_page::print(size_t printed_bytes)
//{
//  for (size_t i = 0; i < printed_bytes; i += 0x10)
//  {
//    // print 16 bytes on each line
//    for (size_t x = 0; i+x < m_mem_info.RegionSize && x < 0x10; x++) 
//    {
//      std::cout << m_buffer.get()[i+x] << " ";
//    }
//
//    std::cout << std::endl;
//  }
//}

void mem_page::print_info()
{
  std::cout << "Page start address: 0x" << CONFIGURE_HEX << reinterpret_cast<uint32_t>(m_mem_info.BaseAddress) << CONFIGURE_DEC << std::endl;
  //std::cout << "Page size " << reinterpret_cast<uint32_t>(m_mem_info.RegionSize) << std::endl;
  std::cout << "Page size " << m_mem_info.RegionSize << std::endl;
}

//uint32_t mem_page::get_base()
//{
//  BaseAddress
//  return (uint32_t)m_base_addr;
//}

uint8_t* mem_page::get_buffer()
{
  return m_buffer.get();
}

uint8_t* mem_page::get_buffer(uint32_t offset)
{
  return &(m_buffer.get()[offset]);
}

std::vector<uint32_t> mem_page::search_page(SIZE_T buff_len, uint8_t *buff)
{
  std::vector<uint32_t> results_vec;
  uint8_t *local_buff = this->get_buffer();
  
  for (SIZE_T x = 0; x <= (m_mem_info.RegionSize - buff_len); x++)
  {
    bool found_match = true;

    for (SIZE_T y = 0; y < buff_len; y++)
    {
      if (buff[y] != local_buff[x+y])
      {
        found_match = false;
        break;
      }
    }

    if (found_match)
      results_vec.push_back(x + reinterpret_cast<uint32_t>(m_mem_info.BaseAddress));
  }

  return results_vec;
}


void mem_page::save_page()
{
  std::ostringstream sstr;

  // build file path+name = ./mem_dump/mem_page_<32_bit_addr>.bin
  sstr << "mem_dump/mem_page_" << std::setfill('0') << std::setw(8) << std::hex << std::uppercase << int(m_mem_info.BaseAddress) << ".bin"; 

  std::string file_path(sstr.str());
  std::ofstream file(file_path.c_str(), std::ios::trunc | std::ios::binary);

  if (!file.is_open())
  {
    std::cout << "Fail to open " << sstr.str() << std::endl;
    return;
  }

  file.write(reinterpret_cast<char*>(m_buffer.get()), m_mem_info.RegionSize);

  //if (file.rdstate() & std::ofstream::badbit)
  if (file.fail())
  {
    std::cout << "Fail while writing " << sstr.str() << std::endl;
  }

  file.close();
}

void mem_page::load_page(std::string file_path)
{
  std::ifstream mem_file(file_path.c_str(), std::ifstream::binary);

  // file should end with <32_bit_addr>.bin - addr must be 8 characters
  std::string addr_str(file_path.substr(file_path.length()-12, file_path.length()-4)); 
  std::cout << "Read addr " << addr_str << std::endl;

  if (!mem_file.is_open()) 
  {
    std::cout << "Fail to read " << file_path << std::endl;
  }

  // get length of file:
  mem_file.seekg (0, mem_file.end);

  m_mem_info.RegionSize = mem_file.tellg();
  mem_file.seekg (0, mem_file.beg);
  m_buffer = std::unique_ptr<uint8_t>(new uint8_t[m_mem_info.RegionSize]());
  if (m_buffer == nullptr)
  {
    std::cout << "Fail to allocate " << m_mem_info.RegionSize << " bytes" << std::endl;
  }

  // read data as a block:
  mem_file.read(reinterpret_cast<char*>(m_buffer.get()),m_mem_info.RegionSize);

  if (mem_file.fail())
    std::cout << "Fail reading file " << file_path << std::endl;
  //std::cout << "Error reading only " << mem_file.gcount() << " could be read";

  mem_file.close();
}

void mem_page::print()
{
  std::cout << "0x" << CONFIGURE_HEX << m_mem_info.BaseAddress << std::endl;
  for (SIZE_T x = 0; x < 0x10; x++)
  {
    std::cout << CONFIGURE_HEX << std::setw(2) << x << CONFIGURE_DEC << " ";
  }
  std::cout << std::endl;

  for (SIZE_T x = 0; x < m_mem_info.RegionSize; x+=0x10)
  {
    for (SIZE_T y = 0; (y < 0x10) && (x + y < m_mem_info.RegionSize); y++)
    {
      std::cout << CONFIGURE_HEX << std::setfill('0') << std::setw(2) << +uint8_t(m_buffer.get()[x+y]) << CONFIGURE_DEC << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}
