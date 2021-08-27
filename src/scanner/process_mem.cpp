#include <stdexcept>
#include <iostream>
#include <string>

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
#if 0
  mem_page *largest_mem_page = NULL;  // test
#endif

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

#if 0
    // test
    if (largest_mem_page == NULL)
      largest_mem_page = m_mem_page_vec.back();

    if (mem_info.RegionSize > largest_mem_page->get_len())
      largest_mem_page = m_mem_page_vec.back();
#endif
  }
#if 0
  // test
  std::cout << "DBG - largest page: " << largest_mem_page->get_len() << std::endl;
  std::cout << "DBG - largest page addr: 0x" << CONFIGURE_HEX << largest_mem_page->get_base() << CONFIGURE_DEC << std::endl;
#endif
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

