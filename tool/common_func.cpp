#ifndef _COMMON_FUNC_HPP_
#define _COMMON_FUNC_HPP_

#include "common_func.h"
#include <string>
#include <cstring>
#include <zconf.h>

//----------------------   工具函数  begin ---------------------------

std::string get_app_fullpath()
{
    char s_buff[1024] = { 0 };
    int len = readlink("/proc/self/exe", s_buff, 1024);

    std::string s_ret = s_buff;

    //去掉 ----> '/程序名'
    int n_reverse_index = s_ret.rfind("/");
    if (n_reverse_index == -1)//未查找成功
    {
        return "";
    }

    //截断
    s_ret.replace(n_reverse_index, s_ret.size() - n_reverse_index, "/");

    return s_ret;
}

//----------------------   工具函数  end  ---------------------------

//----------------------   安全函数  begin ---------------------------

void memcpy_s(void* p_des, unsigned  int n_des_len, const void* p_src, unsigned int n_src_len)
{
    //参数异常
    if (p_des == nullptr || p_src == nullptr || !n_des_len || !n_src_len)
    {
        return;
    }

    //源数据和目的数据长度教研
    if (n_src_len > n_des_len)
    {
        n_src_len = n_des_len;
    }

    //拷贝
    memcpy(p_des, p_src, n_src_len);
}

char* strcpy_s(char* p_des, unsigned int n_des_len, const char* p_src, unsigned int n_src_len)
{
    //参数异常
    if (p_des == nullptr || p_src == nullptr || !n_des_len || !n_src_len)
    {
        return p_des;
    }

    //纠正拷贝的长度
    unsigned  int n_temp_len = strlen((char*)p_src);
    if (n_src_len > n_temp_len)
    {
        n_src_len = n_temp_len;
    }

    //源数据和目的数据长度教研
    if (n_src_len > n_des_len)
    {
        n_src_len = n_des_len;
    }

    strncpy(p_des, p_src, n_src_len);

    return p_des;
}




//----------------------   end   ---------------------------

#endif