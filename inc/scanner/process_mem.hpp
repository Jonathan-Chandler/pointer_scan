#ifndef __PROCESS_MEM_HPP__
#define __PROCESS_MEM_HPP__
#include <windows.h>
#include <vector>
#include "process_info.hpp"
#include "mem_page.hpp"

class Process_Mem
{
  private:
    Process_Info* const m_proc_info;
    std::vector<mem_page*> m_mem_page_vec;

    void allocate_pages();
    void deallocate_pages();
    void refresh_pages();

//    write_process_page()
//    refresh_process_page()

//    add_mem_page()

    // unused
    Process_Mem():m_proc_info(NULL){};
    Process_Mem(const Process_Info& rhs):m_proc_info(NULL){};

  public:
    Process_Mem(Process_Info* const proc_info);
    Process_Mem(Process_Info* const proc_info, mem_page *);
    ~Process_Mem();

    uint32_t get_base_addr();

    uint8_t* get_page_buffer(uint32_t addr);
    int32_t get_page_and_buffer(uint32_t request_addr, mem_page **return_page, uint8_t **return_buffer);

    int32_t get_mem_pointer(mem_page **page);
    int32_t set_mem_pointer(mem_page *page, uint32_t offset, uint32_t len, uint8_t *data);

    std::vector<uint32_t> search_pages(SIZE_T buff_len, uint8_t *buff);
    void print();
};


#endif

