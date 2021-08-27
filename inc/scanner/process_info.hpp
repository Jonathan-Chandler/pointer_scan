#ifndef __PROCESS_INFO_HPP__
#define __PROCESS_INFO_HPP__
#include <windows.h>
#include <string>

class Process_Info
{
  private:
    const bool debug_mode = false;
    const std::string m_window_title;
    const std::string m_executable_name;
    HWND m_window_handle;
    HANDLE m_process_handle;
    DWORD m_process_id;
    DWORD m_process_base_addr;

    void find_window_handle();
    void find_process_id();
    void open_process_handle();
    void find_process_base_addr();

    void close_process_handle();

    // unused
    Process_Info(){};
    Process_Info(const Process_Info& rhs){};
    //process_info& operator=(const process_info& rhs){return process_info::process_info();};

  public:
    Process_Info(const char *window_title);
    Process_Info(const char *window_title, const char *executable_name);
    ~Process_Info();

    void initialize();

    const std::string get_window_title();
    const std::string get_executable_name();
    HWND get_window_handle();
    HANDLE get_process_handle();
    DWORD get_process_id();
    DWORD get_process_base_addr();

    void print();
};


#endif
