#ifndef _COMMON_FUNC_HPP_
#define _COMMON_FUNC_HPP_

#include <string>
#include <cstring>

//----------------------   工具宏  begin ---------------------------

//获取数组长度宏
#define GET_ARRAY_LEN(array) ((sizeof(array))/(sizeof(array[0])))

//输出调试代码具体信息宏
#define DEBUG_CODE_INFO (QString("--->detail: filename=%1 | function=%2 | line=%3<---").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__))


//----------------------   工具宏  end ---------------------------


//----------------------   工具函数  begin ---------------------------

//获取程序的绝对路径
std::string get_app_fullpath();

//----------------------   工具函数  end ---------------------------


//----------------------   安全函数  begin ---------------------------

void memcpy_s(void* p_des, unsigned  int n_des_len, const void* p_src, unsigned int n_src_len);

char* strcpy_s(char* p_des, unsigned int n_des_len, const char* p_src, unsigned int n_src_len);




//----------------------   end   ---------------------------

#endif