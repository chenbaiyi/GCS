//
// Created by chenbaiyi on 18-7-16.
//

#include "redisacc.h"


REDISACC::RedisManager::RedisManager()
        : m_redisContext(nullptr), m_mode(REDISMANAGER_MODE::SYN_MODE), m_valid(false)
{
}

REDISACC::RedisManager::~RedisManager()
{
}

/*
 *  des: 连接redis
 *  param:
 *      ip:　ip地址
 *      port:   端口
 *      asyn:　模式（同步／异步）
 *  ret:
 *      nullptr标示执行失败，否则直接返回redisReply结构　需要外部释放
 */
bool REDISACC::RedisManager::connectRedis(const std::string& ip, uint32_t port, REDISMANAGER_MODE asynMode)
{
    if (!m_lock.TryLock())
    {
        return false;
    }

    //异步模式连接
    if (asynMode == REDISMANAGER_MODE::ASYN_MODE)
    {

    }
    else//同步
    {
        //连接
        m_redisContext = redisConnect(ip.c_str(), port);
        //检查连接返回状态
        if (m_redisContext == nullptr || m_redisContext->err)
        {
            return false;
        }

        //记录数据　后续命令执行失败需要创建新的连接
        m_mode = asynMode;
        m_ip = ip;
        m_port = port;
        m_valid = true;
    }

    m_lock.Unlock();
    return true;
}

/*
 *  des: 断开连接
 *  param:
 *  ret:
 */
void REDISACC::RedisManager::disconnecRedis()
{
    m_lock.Lock();
    if (m_redisContext)
    {
        redisFree(m_redisContext);
        m_redisContext = nullptr;
    }
    m_lock.Unlock();
}

/*
 *  des: 执行命令
 *  param:
 *      cmd:　命令数据
 *      errNo:　错误码
 *      cmdLen:　命令的长度　主要针对保存非字符数据
 *  ret:
 *      nullptr标示执行失败，否则直接返回redisReply结构　需要外部释放
 */
redisReply *REDISACC::RedisManager::execCmd(const std::string& cmd, int32_t& errNO, int cmdLen)
{
    //尝试加锁　加锁失败直接返回
    if (!m_lock.TryLock())
    {
        errNO = REDISACC::REDISMANAGER_ERR::TRYLOCK_ERR;
        return nullptr;
    }

    //初始化返回的错误码
    errNO = 0;

    if (m_redisContext == nullptr)
    {
        errNO = REDISACC::REDISMANAGER_ERR::DISCONNECT_ERR;
        return nullptr;
    }

    redisReply* reply = nullptr;
    if (cmdLen != -1)
    {
        char* arrCmd = new char[cmd.size() + 1];
        memset(arrCmd, 0, cmd.size() + 1);
        memcpy(arrCmd, cmd.c_str(), cmd.size());

        reply = (redisReply*)redisCommand(m_redisContext, arrCmd);
    }
    else
    {
        reply = (redisReply*)redisCommand(m_redisContext, cmd.c_str());
    }

    if (reply == nullptr)//返回空　上下文不能重用，必须重新建立连接
    {
        printf("redisCommand ret null, err: %s\n", m_redisContext->errstr);

        //释放连接　重新创建
        redisFree(m_redisContext);
        m_redisContext = nullptr;
        m_redisContext = redisConnect(m_ip.c_str(), m_port);
        //检查连接返回状态
        if (m_redisContext == nullptr || m_redisContext->err)
        {
            printf("context reset, reconnect failed\n");
        }

        errNO = REDISACC::REDISMANAGER_ERR::REPLY_NULL_ERR;
        m_lock.Unlock();
        return nullptr;
    }
    else//检测回复状态是否在正常　不正常直接不处理
    {
        switch (reply->type)
        {
            case REDIS_REPLY_ERROR:
                printf("REDIS_REPLY_ERROR: %s\n", reply->str);
                //错误则需要调用释放接口
                freeReplyObject(reply);
                reply = nullptr;

                errNO = REDISACC::REDISMANAGER_ERR::REPLY_NOT_NULL_ERR;
                break;
        }
    }

    m_lock.Unlock();
    return reply;
}

/*
 *  des: 检查该执行器是否有效
 *  param:
 *  ret:
 *      true标识有效，false标识无效
 */
bool REDISACC::RedisManager::isValid()
{
    return m_valid;
}

redisReply *REDISACC::RedisManager::execCmdArgv(int argc, const char **argv, const size_t *argvLen, int32_t& errNo)
{
    //尝试加锁　加锁失败直接返回
    if (!m_lock.TryLock())
    {
        errNo = REDISACC::REDISMANAGER_ERR::TRYLOCK_ERR;
        return nullptr;
    }

    //初始化返回的错误码
    errNo = 0;

    if (m_redisContext == nullptr)
    {
        errNo = REDISACC::REDISMANAGER_ERR::DISCONNECT_ERR;
        return nullptr;
    }

    //执行命令
    redisReply* reply = (redisReply*)redisCommandArgv(m_redisContext, argc, argv, argvLen);
    if (reply == nullptr)//返回空　上下文不能重用，必须重新建立连接
    {
        printf("redisCommand ret null, err: %s\n", m_redisContext->errstr);

        //释放连接　重新创建
        redisFree(m_redisContext);
        m_redisContext = nullptr;
        m_redisContext = redisConnect(m_ip.c_str(), m_port);
        //检查连接返回状态
        if (m_redisContext == nullptr || m_redisContext->err)
        {
            printf("context reset, reconnect failed\n");
        }

        errNo = REDISACC::REDISMANAGER_ERR::REPLY_NULL_ERR;
        m_lock.Unlock();
        return nullptr;
    }
    else//检测回复状态是否在正常　不正常直接不处理
    {
        switch (reply->type)
        {
            case REDIS_REPLY_ERROR:
                printf("REDIS_REPLY_ERROR: %s\n", reply->str);
                //错误则需要调用释放接口
                freeReplyObject(reply);
                reply = nullptr;

                errNo = REDISACC::REDISMANAGER_ERR::REPLY_NOT_NULL_ERR;
                break;
        }
    }

    m_lock.Unlock();
    return reply;

}

