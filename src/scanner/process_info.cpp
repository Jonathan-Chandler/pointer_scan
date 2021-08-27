#include <stdexcept>
#include <iostream>
#include <string>
#include <windows.h>
#include <TlHelp32.h>

#include "utils.hpp"
#include "process_info.hpp"

Process_Info::Process_Info(const char *window_title)
  : Process_Info(window_title, std::string(std::string(window_title) + std::string(".exe")).c_str())
{
}

Process_Info::Process_Info(const char *window_title, const char *executable_title)
  : m_window_title(window_title)
  , m_executable_name(executable_title)
{
}

Process_Info::~Process_Info()
{
  try
  {
    close_process_handle();
  }
  catch (std::exception& e)
  {
    std::cerr << "Error closing process handle: " << e.what() << std::endl;
  }
  catch (...) {}
}

void Process_Info::initialize()
{
  find_window_handle();

  find_process_id();

  open_process_handle();

  find_process_base_addr();
}

void Process_Info::find_window_handle()
{
  if (strcmp(m_window_title.c_str(), "") == 0)
    throw std::runtime_error(std::string("Process_Info: Blank m_window_title"));

  m_window_handle = FindWindow(0, m_window_title.c_str());
  if (this->debug_mode)
  {
    DEBUG_PRINT_LOG("window_handle = 0x" << CONFIGURE_HEX << reinterpret_cast<int>(m_window_handle) << CONFIGURE_DEC)
  }

  if (m_window_handle == 0)
    throw std::runtime_error(std::string(m_window_title) + std::string(" window not found"));
}

void Process_Info::find_process_id()
{
  PROCESSENTRY32 entry;
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  entry.dwSize = sizeof(PROCESSENTRY32);

  if (Process32First(snapshot, &entry) == TRUE)
  {
    while (Process32Next(snapshot, &entry) == TRUE)
    {
      if (stricmp(entry.szExeFile, m_executable_name.c_str()) == 0)
      {  
        m_process_id = entry.th32ProcessID;
        if (this->debug_mode)
        {
          DEBUG_PRINT_LOG("process_id = " << m_process_id);
        }
        break;
      }
    }
  }

  CloseHandle(snapshot);
}

void Process_Info::open_process_handle()
{
  if (m_process_id == 0)
    throw std::runtime_error(std::string("open_process_handle: process ID was 0"));

  m_process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_process_id);
  if (this->debug_mode)
  {
    DEBUG_PRINT_LOG(std::string(std::string("process_handle = ") + std::to_string(reinterpret_cast<int>(m_process_handle))));
  }

  if (m_process_handle == 0)
    throw std::runtime_error(std::string("Failed to get handle for process"));
}

void Process_Info::find_process_base_addr()
{
  uintptr_t modBaseAddr = 0;
  HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE, m_process_id);
  m_process_base_addr = 0;

  if (hSnap != INVALID_HANDLE_VALUE)
  {
    MODULEENTRY32 modEntry;
    modEntry.dwSize = sizeof(modEntry);
    if (Module32First(hSnap, &modEntry))
    {
      do
      {
        if (!_stricmp(modEntry.szModule, m_executable_name.c_str()))
        {
          modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
          
          if (this->debug_mode)
          {
            DEBUG_PRINT_LOG("Found " << m_executable_name << " at " << std::to_string(reinterpret_cast<int>(m_window_handle)) << std::endl);
          }

          m_process_base_addr = modBaseAddr;
          CloseHandle(hSnap);
          return;
        }
      } while (Module32Next(hSnap, &modEntry));
    }
    else
    {
      DEBUG_PRINT_FILE_LINE();
      debug_print_last_win_error();
      CloseHandle(hSnap);
      throw std::runtime_error(std::string("Failed to get Module32First"));
    }
  }
  else
  {
    DEBUG_PRINT_FILE_LINE();
    debug_print_last_win_error();
    CloseHandle(hSnap);
    throw std::runtime_error(std::string("Failed to CreateToolhelp32Snapshot"));
  }

  // search failed to match
  CloseHandle(hSnap);
  throw std::runtime_error(std::string("Failed to find base address for ") + m_executable_name);
}

void Process_Info::close_process_handle()
{
  if (m_process_handle == 0)
    throw std::runtime_error(std::string("Process handle was zero"));

  CloseHandle(m_process_handle);

  m_process_handle = 0;
}

void Process_Info::print()
{
  std::cout << "Address process Info: " << this << std::endl;
  std::cout << "Window title: " << m_window_title << std::endl;
  std::cout << "Executable name: " << m_executable_name << std::endl;
  std::cout << "Window handle: " << m_window_handle << std::endl;
  std::cout << "Process handle: " << m_process_handle << std::endl;
  std::cout << "Process id: " << m_process_id << std::endl;
  std::cout << "Process base_addr: 0x" << CONFIGURE_HEX << static_cast<uint32_t>(m_process_base_addr) << CONFIGURE_DEC << std::endl;
  std::cout << std::endl;
}

const std::string Process_Info::get_window_title()
{
  return m_window_title;
}

const std::string Process_Info::get_executable_name()
{
  return m_executable_name;
}

HWND Process_Info::get_window_handle()
{
  return m_window_handle;
}

HANDLE Process_Info::get_process_handle()
{
  return m_process_handle;
}

DWORD Process_Info::get_process_id()
{
  return m_process_id;
}

DWORD Process_Info::get_process_base_addr()
{
  return m_process_base_addr;
}

