//
// Created by chenbaiyi on 18-7-16.
//

#ifndef TESTTHRIFT_REDISACC_H
#define TESTTHRIFT_REDISACC_H

#include "../GCCondBindMutex.h"

#include <iostream>
#include <string.h>
#include <stdint.h>
#include <hiredis/hiredis.h>

namespace REDISACC
{
    enum REDISMANAGER_MODE
    {
        ASYN_MODE,
        SYN_MODE
    };

    enum REDISMANAGER_ERR
    {
        NO_ERR = 0,//没有错误
        DISCONNECT_ERR,//断开了连接
        TRYLOCK_ERR,//加锁失败
        REPLY_NULL_ERR,//回复空值错误
        REPLY_NOT_NULL_ERR//回复非空值错误
    };

    class RedisManager
    {
    public:
        RedisManager();
        ~RedisManager();

        bool connectRedis(const std::string& ip, uint32_t port, REDISMANAGER_MODE asynMode = SYN_MODE);
        void disconnecRedis();
        redisReply* execCmd(const std::string& cmd, int32_t& errNO, int cmdLen = -1);
        redisReply* execCmdArgv(int argc, const char** argv, const size_t* argvLen, int32_t& errNo);
        bool isValid();


    private:
        bool m_mode;//模式　１标识异步　０标识同步(默认)
        bool m_valid;

        redisContext* m_redisContext;//连接返回的上下文

        GCCondBindMutex m_lock;//内部锁　　
        std::string m_ip;
        uint32_t  m_port;
    };
}


#endif //TESTTHRIFT_REDISACC_H
