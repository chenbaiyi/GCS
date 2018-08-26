//
// Created by chenbaiyi on 18-8-2.
//

#include <sstream>
#include <zconf.h>
#include "redisstd.h"
#include "redisacc.h"

/*
 **@des:
 **@param:
 **@ret:
 **@date:
 **@author:
 */
REDISSTD::RedisLocker::RedisLocker(std::string lockerName, const uint32_t& existTime, REDISACC::RedisManager& redisManager)
    : m_lockTime(0), m_exsitTime(existTime), m_redisManager(redisManager), m_lockerName(lockerName)
{

}

REDISSTD::RedisLocker::~RedisLocker()
{
    //当前时间 > 锁定时间+有效时间 并且（枷锁失败或者有效时为永久)
    time_t currTime = time(nullptr);
    if ((!m_lockTime || m_exsitTime) && currTime >= m_exsitTime + m_lockTime)
    {
        return;//直接返回
    }

    //解锁
   unLock();
}

/*
 **@des:尝试加锁 立即返回
 **@param:
 **@ret:
 *  true表示加锁成功 false表示加锁失败
 **@date:
 **@author:
 */
bool REDISSTD::RedisLocker::tryLock()
{
    std::stringstream sstr;
    sstr << "SETNX " << m_lockerName << " REDISLOCKER";
    std::string cmd = sstr.str();
    
    int errNo = 0;
    redisReply* reply = m_redisManager.execCmd(cmd.c_str(), errNo);
    if (errNo == REDISACC::NO_ERR && reply && reply->integer)//枷锁成功
    {
        //释放
        freeReplyObject(reply);
        reply = nullptr;

        //存在时间为0  表示永久存在
        if (!m_exsitTime)
        {
            m_lockTime = time(nullptr);
            return true;
        }

        //设置超时时间
        sstr.str("");
        if (m_exsitTime)
        {
            sstr << "EXPIRE " << m_lockerName << " " << m_exsitTime;
        }
        cmd = sstr.str();

        redisReply* reply = m_redisManager.execCmd(cmd.c_str(), errNo);
        if (errNo == REDISACC::NO_ERR && reply && reply->integer)//设置有效时间成功
        {
            //释放
            freeReplyObject(reply);
            reply = nullptr;

            m_lockTime = time(nullptr);
            return true;//加锁成功
        }
    }

    //释放
    if(reply)
    {
        freeReplyObject(reply);
        reply = nullptr;
    }

    //加锁失败
    return false;
}

/*
 **@des:加锁  阻塞，只有等到超时才会返回
 **@param:
 *      waitTime：超时时间---ms
 **@ret:
 *      true加锁成功  false加锁失败
 **@date:
 **@author:
 */
bool REDISSTD::RedisLocker::lock(uint32_t waitTime)
{
    if (waitTime < 5)
    {
        waitTime = 5;
    }

    while (waitTime)
    {
        if (tryLock())
        {
            return true;
        }

        usleep(1000 * 5);
        waitTime -= 5;
    }

    return false;//超时触发  加锁失败
}

/*
 **@des:解锁
 **@param:
 **@ret:
 **@date:
 **@author:
 */
void REDISSTD::RedisLocker::unLock()
{
    //删除锁
    std::stringstream sstr;
    sstr << "DEL " << m_lockerName;
    std::string cmd = sstr.str();

    int errNo = 0;
    redisReply* reply = m_redisManager.execCmd(cmd.c_str(), errNo);

    //如果是加锁失败 则等待加锁成功
    while (errNo != REDISACC::NO_ERR && errNo == REDISACC::TRYLOCK_ERR)//执行
    {
        if(reply)
        {
            freeReplyObject(reply);
            reply = nullptr;
        }

        usleep(1000 * 2);
        printf("RedisLocker unLock try again\n");
        reply = m_redisManager.execCmd(cmd.c_str(), errNo);
    }

    if (errNo != REDISACC::NO_ERR)
    {
        printf("RedisLocker unLock failed\n");
    }
}
