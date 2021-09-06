#include <stdexcept>
#include <iostream>
#include <string>
#include <filesystem>

// #include "debug.h"
#include "utils.hpp"
#include "process_mem.hpp"
#include "process_info.hpp"

Process_Mem::Process_Mem(Process_Info* const proc_info)
  : m_proc_info(proc_info)
{
  allocate_pages();
  //refresh_pages();
}

Process_Mem::Process_Mem(std::string directory)
  : m_proc_info(nullptr),
    m_page_dir(directory)
{
  load_pages();
  //refresh_pages();
}

Process_Mem::~Process_Mem()
{
  deallocate_pages();
}

void Process_Mem::print()
{
  std::cout << "address of Process_Mem: " << this << std::endl;
  std::cout << "address of Process_Info: " << m_proc_info << std::endl;
  std::cout << "process_window: " << m_proc_info->get_window_handle() << std::endl;
  std::cout << "process_handle: " << m_proc_info->get_process_handle() << std::endl;
  std::cout << "process_id: " << m_proc_info->get_process_id() << std::endl;
  
  for (std::vector<mem_page*>::iterator it = m_mem_page_vec.begin(); it != m_mem_page_vec.end(); ++it)
  {
      (*it)->print_info();
  }
}

void Process_Mem::allocate_pages()
{
  MEMORY_BASIC_INFORMATION mem_info;
  mem_page *temp_page;
  unsigned char *addr = 0;

  while (true)
  {
    // read memory information for block starting at addr
    if (VirtualQueryEx(m_proc_info->get_process_handle(), addr, &mem_info, sizeof(mem_info)) == 0)
    {
      //std::cout << "Ended scan at address: 0x" << CONFIGURE_HEX << (uint32_t)addr << CONFIGURE_DEC << std::endl;
      break;
    }

    // address of next page to copy
    addr += mem_info.RegionSize;

    // ignore uncommitted / non-writable / guarded pages
    if (!(mem_info.State & MEM_COMMIT)
      ||  !(mem_info.Protect & MEMINFO_PROTECT_IS_WRITABLE)
      ||  (mem_info.Protect & PAGE_GUARD))
    {
      continue;
    }

    // allocate space to store this page
    temp_page = new mem_page(m_proc_info, &mem_info);

    // if state/protect check or ReadProcessMemory failed
    if (temp_page->refresh_page())
    {
      // skip this page and delete allocated memory
      delete temp_page;
      continue;
    }

    // add new page to list of pages
    m_mem_page_vec.push_back(temp_page);
  }
}

void Process_Mem::deallocate_pages()
{
  auto it = m_mem_page_vec.begin();

  while (it != m_mem_page_vec.end())
  {
    it = m_mem_page_vec.erase(it);
  }

  m_mem_page_vec.clear();
}

void Process_Mem::refresh_pages()
{
  for (size_t i = 0; i < m_mem_page_vec.size(); ++i) 
  {
    if (m_mem_page_vec[i]->refresh_page())
    {
      std::cout << "Fail to refresh page " << i << std::endl;

      // delete page
      
    }
  }
}

uint32_t Process_Mem::get_base_addr()
{
  return m_proc_info->get_process_base_addr();
}

uint8_t* Process_Mem::get_page_buffer(uint32_t addr)
{
  for (std::vector<mem_page*>::iterator it = m_mem_page_vec.begin(); it != m_mem_page_vec.end(); ++it)
  {
    uint32_t page_begin = reinterpret_cast<uint32_t>((*it)->get_base_addr());
    uint32_t page_end = page_begin + (*it)->get_len();

    if (addr >= page_begin && addr < page_end)
    {
      return (*it)->get_buffer(addr - page_begin);
    }
  }

  std::cout << "Failed to find addr: 0x" << CONFIGURE_HEX << addr << CONFIGURE_DEC << std::endl;
  return NULL;
}

std::vector<uint32_t> Process_Mem::search_pages(SIZE_T buff_len, uint8_t *buff)
{
  std::vector<uint32_t> result_vec;

  for (auto it = m_mem_page_vec.begin(); it != m_mem_page_vec.end(); ++it)
  {
    std::vector<uint32_t> page_result = (*it)->search_page(buff_len, buff);

    // add results for current page to end of vector
    if (!page_result.empty())
    {
      result_vec.insert(result_vec.end(), page_result.begin(), page_result.end());
    }
  }

  return result_vec;
}

void Process_Mem::save_pages()
{
  std::cout << "Saving pages" << std::endl;

  for (auto it = m_mem_page_vec.begin(); it != m_mem_page_vec.end(); ++it)
  {
    (*it)->save_page();
  }

  std::cout << "Saved pages" << std::endl;
}

void Process_Mem::load_pages()
{
  std::string path = "mem_dump";

  for (const auto & entry : std::filesystem::directory_iterator(path))
  {
    mem_page *temp_page;
    std::string file_str(entry.path().string());
    std::string addr_str(file_str.substr(file_str.length()-12, 8)); 
    uint32_t base_addr = stoi(addr_str, 0, 16);

    std::cout << "Addr " << CONFIGURE_HEX << base_addr << CONFIGURE_DEC << std::endl;
    std::cout << "Loading " << entry.path() << std::endl;

    temp_page = new mem_page(reinterpret_cast<PVOID>(base_addr), file_str);

    m_mem_page_vec.push_back(temp_page);
  }
}

//int32_t Process_Mem::get_page_and_buffer(uint32_t request_addr, mem_page **return_page, uint8_t **return_buffer)
//{
//  if (return_page == nullptr || return_buffer == nullptr)
//  {
//    throw std::runtime_error(std::string(__FILE__) + std::string("::") + std::string(__FUNCTION__) + std::string("::") + std::to_string(__LINE__) + std::string("::Null return pointer"));
//  }
//
//  *return_page = nullptr;
//  *return_buffer = nullptr;
//
//  for (std::vector<mem_page*>::iterator it = m_mem_page_vec.begin(); it != m_mem_page_vec.end(); ++it)
//  {
//    uint32_t page_begin = (*it)->get_base_addr();
//    uint32_t page_end = page_begin + (*it)->get_len();
//
//    if (request_addr >= page_begin && request_addr < page_end)
//    {
//      *return_buffer = (*it)->get_buffer(request_addr - page_begin);
//      *return_page = *it;
//      return 0;
//    }
//  }
//
//  std::cout << "Failed to find request_addr: 0x" << CONFIGURE_HEX << request_addr << CONFIGURE_DEC << std::endl;
//  return -1;
//}

