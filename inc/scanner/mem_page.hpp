#ifndef __MEM_PAGE_HPP__
#define __MEM_PAGE_HPP__
#include <windows.h>
#include <memory>
#include <vector>
#include "process_info.hpp"
//#include <stdint.h>

#define MEMINFO_PROTECT_IS_WRITABLE   (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)
#define MEMINFO_PROTECT_IS_READABLE   (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)
#define MEMINFO_PROTECT_IS_EXECUTABLE (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)

/*
typedef struct _MEMORY_BASIC_INFORMATION {
  PVOID  BaseAddress;
  PVOID  AllocationBase;
  DWORD  AllocationProtect;
  WORD   PartitionId;
  SIZE_T RegionSize;
  DWORD  State;
  DWORD  Protect;
  DWORD  Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;
*/

class mem_page
{
  private:
    Process_Info* const m_proc_info;      // process id
    MEMORY_BASIC_INFORMATION m_mem_info;  // memory page information
    std::unique_ptr<uint8_t> m_buffer;

    int copy_page();                      // copy data at m_mem_info.BaseAddress to m_buffer
    int initialize();

    // unused
    mem_page(): m_proc_info(nullptr){};
    mem_page(const mem_page& rhs): m_proc_info(nullptr){};

  public:
    mem_page(Process_Info *process_info, MEMORY_BASIC_INFORMATION *p_mem_info);
    mem_page(PVOID BaseAddress, std::string file_path);
    ~mem_page();

    // update buffer
    int refresh_page();

    Process_Info* const get_process_info(){return m_proc_info;};
    PVOID get_base_addr();
    uint32_t get_len();
    uint8_t* get_buffer();
    uint8_t* get_buffer(uint32_t offset);
    std::vector<uint32_t> search_page(SIZE_T buff_len, uint8_t *buff);
    void print();
    void print(size_t printed_bytes);
    void print_info();
};


#endif


