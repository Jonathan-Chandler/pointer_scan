#ifndef __UTILS_HPP__
#define __UTILS_HPP__
#include <iostream>

#define CONFIGURE_HEX std::hex<<std::uppercase
#define CONFIGURE_DEC std::nouppercase<<std::dec

#define DEBUG_PRINT_FILE_LINE() \
  std::cout << "DEBUG::" << __FILE__ << "::" << __LINE__ << std::endl;
//  std::cout << reinterpret_cast<const char*>(__FILE__) << "::" << static_cast<uint32_t>(__LINE__) << std::endl;

#define DEBUG_PRINT_LOG(error_string) \
  std::cout << "DEBUG::" << __FILE__ << "::" << __LINE__ << "::" << error_string << std::endl;


void debug_print_last_win_error();

#endif // ifndef __UTILS_HPP__
