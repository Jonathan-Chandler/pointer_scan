#include <windows.h>
#include <stdio.h>
#include "utils.hpp"

// inline void debug_print_last_win_error()
//void debug_print_last_win_error()
//{
//  LPVOID lpMsgBuf;
//  DWORD dw = GetLastError();
//
//  printf("%s::%d::", __FILE__, __LINE__);
//  printf("Windows error: 0x%lX\n", GetLastError());
//  FormatMessage(
//      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
//      FORMAT_MESSAGE_FROM_SYSTEM |
//      FORMAT_MESSAGE_IGNORE_INSERTS,
//      NULL,
//      dw,
//      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//      (LPTSTR) &lpMsgBuf,
//      0, NULL );
//  printf((char*)lpMsgBuf);
//}

void debug_print_last_win_error()
{
  LPVOID lpMsgBuf;
  DWORD dw = GetLastError();

//  printf("%s::%d::", __FILE__, __LINE__);
  printf("Windows error: 0x%lX\n", GetLastError());
  FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      dw,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &lpMsgBuf,
      0, NULL );
  printf((char*)lpMsgBuf);
}
