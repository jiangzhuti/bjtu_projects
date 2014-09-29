#ifndef LOG_DEF_H
#define LOG_DEF_H

#include <sys/types.h>
#include <time.h>

/* gcc 编译动态库， 函数默认是导出的。这个宏用于控制函数是不是导出。 */
#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllimport))
    #else
      #define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define DLL_PUBLIC __attribute__ ((visibility ("default")))
    #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_PUBLIC
    #define DLL_LOCAL
  #endif
#endif

/* 文件打开模式 */
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/* index文件每条记录的长度：时间 + 偏移量 + 描述部分长度 + log总长度 */
#define INDEX_RECORD_SIZE (sizeof(time_t) + sizeof(off_t) + sizeof(int)/*desp_len*/ + sizeof(int)/*total_len*/)

/* 指针参数的性质 */
#define IN
#define OUT

#endif // LOG_DEF_H
