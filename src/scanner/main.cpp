#include <cassert>
#include <iostream>
#include <exception>
#include <stdint.h>
#include <windows.h>
#include "process_info.hpp"
#include "process_mem.hpp"
#include "utils.hpp"
#include "search_nearby.hpp"

// #include "debug.h"
// #include "memory_scan.h"
// #include "scan_results.h"
// #include "cli.h"

uint32_t addr1 = 0x21BD5E10;
uint32_t addr2 = 0x21CD1410;

int load_file()
{
  Process_Mem *proc_mem;

  try
  {
    proc_mem = new Process_Mem(std::string("mem_dump"));
  }
  catch (std::exception& e)
  {
    std::cerr << "Error getting process mem: " << e.what() << std::endl;
  }

  delete proc_mem;
  return 0;
}

int main(int argc, char *argv[])
{
  Process_Info *proc_info;
  Process_Mem *proc_mem;
  //Search_Nearby *near_search;
  // uint32_t value = 0x21D3A3C8;
  // SIZE_T buf_len = sizeof(value);
  //std::vector<uint32_t> search_vector;

  // load from saved memory dump
  if (argc == 1)
  {
    return load_file();
  }

  proc_info = new Process_Info(argv[1]);

  try
  {
    proc_info->initialize();
  }
  catch (std::exception& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    exit(-1);
  }

  try
  {
    proc_mem = new Process_Mem(proc_info);
  }
  catch (std::exception& e)
  {
    std::cerr << "Error getting process mem: " << e.what() << std::endl;
  }

  proc_mem->save_pages();

#if 0
  search_vector.push_back(addr1);
  search_vector.push_back(addr2);

  near_search = new Search_Nearby(proc_mem, search_vector);
  near_search->start_search();
  near_search->print_search_results();
  near_search->print_least_distance();


  delete near_search;
#endif

  delete proc_mem;
  delete proc_info;

//  if ((retval = mem_scan_init(&this_scan, argv[1])) < 0)
//  {
//    debug_error("initialization fails: %d", retval);
//    exit(-1);
//  }
//
//  if ((retval = mem_scan_search(this_scan, sizeof(value), (uint8_t*) &value)) < 0)
//  {
//    debug_error("initialization fails: %d", retval);
//    exit(-1);
//  }

//  mem_scan_print(this_scan);
//  mem_scan_destroy(&this_scan);

//
//  rc = memscan_create(procInfo);
//  if (rc != 0)
//    printf("rc: %d\n", rc);
//
//  rc = memscan_update(procInfo);
//  if (rc != 0)
//    printf("rc: %d\n", rc);
//
//  memscan_search(procInfo, (uint8_t*)&value, sizeof(value));
//
//  memscan_dump_results(procInfo);
//
//  memscan_destroy(&procInfo);
//
//  cli_main(argv[1]);
//
  return 0;
}

// VirtualQueryEx
// ReadProcessMemory
// WriteProcessMemory
