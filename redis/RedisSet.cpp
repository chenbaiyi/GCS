//
// Created by chenbaiyi on 18-7-19.
//

#include "redisstd.h"
#include <hiredis/hiredis.h>

REDISSTD::RedisSet::RedisSet(std::string setName, REDISACC::RedisManager &redisManager)
        : m_redisManager(redisManager)
{
    m_valid = false;//默认无效

    //hashmap的名称　即对应redis的key
    if (setName.size() && m_redisManager.isValid())
    {
        //检查该set的key值是否已经存在了
        int32_t err = 0;
        std::string cmd = "TYPE " + setName;//type key

        redisReply* reply = redisManager.execCmd(cmd, err);
        if (err == REDISACC::NO_ERR && reply != nullptr && reply->str)//执行成功
        {
            //已存在并且该key值的类型为set
            if (!strcmp("set", reply->str))
            {
                m_valid = true;//标志置为true
                m_setName = setName;
            }
            else if (!strcmp("none", reply->str))//或者该key不存在 则新建一个并初始值为默认 [defalt_value]
            {
                freeReplyObject(reply);
                reply = nullptr;
                cmd.empty();
                cmd = "SADD ";
                cmd.append(setName);
                cmd.append(" ");
                cmd.append(SET_DEFAULT_VALUE);

                reply = redisManager.execCmd(cmd, err);
                if (err == REDISACC::NO_ERR && reply != nullptr)//执行成功
                {
                    m_valid = true;//标志置为true
                    m_setName = setName;
                }
            }

            if (reply != nullptr)
            {
                freeReplyObject(reply);//释放
                reply = nullptr;
            }
        }

        if (reply != nullptr)
        {
            freeReplyObject(reply);//释放
            reply = nullptr;
        }
    }
}

REDISSTD::RedisSet::~RedisSet()
{

}

/*
 *  des: 向集合添加数据
 *  param:
 *      value:　待添加的数据
 *  ret:　
 *      false标识添加失败　　true标识添加成功
 */
bool REDISSTD::RedisSet::add(std::string& value)
{
    //检查该set,参数,命令执行器是否有效
    if (!m_valid || !value.size() || !m_redisManager.isValid())
    {
        return false;
    }

    //添加数据
    int32_t err = 0;
    std::string cmd = "SADD ";
    cmd.append(m_setName);
    cmd.append(" ");
    cmd.append(value);//SADD key value

    redisReply* reply = m_redisManager.execCmd(cmd, err);
    if (reply != nullptr)//执行成功
    {
        freeReplyObject(reply);
        reply = nullptr;

        if (err == REDISACC::NO_ERR)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

/*
 *  des: 从集合中移除数据
 *  param:
 *      value:　待移除的数据
 *  ret:　
 *      false标识移除失败　　true标识移除成功
 */
bool REDISSTD::RedisSet::remove(std::string& value)
{
    //检查该set,参数,命令执行器是否有效
    if (!m_valid || !value.size() || !m_redisManager.isValid())
    {
        return false;
    }

    //添加数据
    int32_t err = 0;
    std::string cmd = "SREM ";
    cmd.append(m_setName);
    cmd.append(" ");
    cmd.append(value);//SPOP key value

    redisReply* reply = m_redisManager.execCmd(cmd, err);
    if (reply != nullptr)//执行成功
    {
        freeReplyObject(reply);
        reply = nullptr;

        if (err == REDISACC::NO_ERR)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

/*
 *  des: 返回集合所有的成员
 *  param:
 *  ret:　返回保存数据的链表
 */
std::list<std::string> REDISSTD::RedisSet::allValues()
{
    //检查该set,参数,命令执行器是否有效
    if (!m_valid || !m_redisManager.isValid())
    {
        return std::list<std::string>();
    }

    //返回链表
    std::list<std::string> valueList;

    //添加数据
    int32_t err = 0;
    std::string cmd = "SMEMBERS " + m_setName;//SMEMBERS key
    redisReply* reply = m_redisManager.execCmd(cmd, err);
    if (err == REDISACC::NO_ERR && reply != nullptr)//执行成功
    {
        redisReply* tmpReply = nullptr;
        for (int i = 0 ;i < reply->elements; i++)//拷贝所有的value值
        {
            tmpReply = reply->element[i];
            if (tmpReply != nullptr && (tmpReply->str != nullptr))
            {
                if (strcmp(SET_DEFAULT_VALUE, tmpReply->str))
                {
                    valueList.push_back(std::string(tmpReply->str, tmpReply->len));
                }
            }
            else
            {
                break;
            }
        }

        freeReplyObject(reply);
        reply = nullptr;
    }

    if (reply != nullptr)
    {
        freeReplyObject(reply);//释放
        reply = nullptr;
    }

    return valueList;
}

/*
 *  des: 返回集合的大小
 *  param:
 *  ret:　集合的具体大小
 */
uint32_t REDISSTD::RedisSet::size()
{
    //检查该set,参数,命令执行器是否有效
    if (!m_valid || !m_redisManager.isValid())
    {
        return 0;
    }

    int size = 0;

    //添加数据
    int32_t err = 0;
    std::string cmd = "SCARD " + m_setName + std::string(" ");//SCARD key

    redisReply* reply = m_redisManager.execCmd(cmd, err);
    if (err == REDISACC::NO_ERR && reply != nullptr)//执行成功
    {
        size = reply->integer;
        freeReplyObject(reply);
        reply = nullptr;
    }

    if (reply != nullptr)
    {
        freeReplyObject(reply);//释放
        reply = nullptr;
    }

    return size - 1;
}

/*
 *  des: 判断集合中是否存在某个成员
 *  param:
 *      value:　待验证的数据
 *  ret:　
 *      false标识不存在　　true标识存在
 */
bool REDISSTD::RedisSet::isExist(std::string& value)
{
    //检查该set,参数,命令执行器是否有效
    if (!m_valid || !m_redisManager.isValid())
    {
        return 0;
    }

    //添加数据
    int32_t err = 0;
    std::string cmd = "SISMEMBER ";
    cmd.append(m_setName);
    cmd.append(value);//SISMEMBER key value

    redisReply* reply = m_redisManager.execCmd(cmd, err);
    if (err == REDISACC::NO_ERR && reply != nullptr)//执行成功
    {
        if (reply->integer == 1)
        {
            freeReplyObject(reply);
            reply = nullptr;

            return true;
        }
        else
        {
            freeReplyObject(reply);
            reply = nullptr;

            return false;
        }
    }

    if (reply != nullptr)
    {
        freeReplyObject(reply);//释放
        reply = nullptr;
    }

    return false;
}

/*
 *  des: 清空集合
 *  param:
 *  ret:　
 */
void REDISSTD::RedisSet::empty()
{
    //检查该set,参数,命令执行器是否有效
    if (!m_valid || !m_redisManager.isValid())
    {
        return;
    }

    //获取所有的field
    std::list<std::string> valueList = allValues();

    //添加数据
    int32_t err = 0;
    std::string cmd = "";
    redisReply* reply = nullptr;
    for (std::string& value : valueList)
    {
        cmd.empty();
        cmd = "SREM ";
        cmd.append(m_setName);
        cmd.append(" ");
        cmd.append(value);

        reply = m_redisManager.execCmd(cmd, err);
        if (err == REDISACC::NO_ERR && reply != nullptr)//执行成功
        {
            freeReplyObject(reply);
            reply = nullptr;
        }

        if (reply != nullptr)
        {
            freeReplyObject(reply);//释放
            reply = nullptr;
        }
    }
}

/*
 *  des: 该集合是否有效　
 *  param:
 *  ret:　
 *      false表示无效　　true表示有效
 */
bool REDISSTD::RedisSet::isValid()
{
    return m_valid;
}

/*
 *  des: 随机从集合中返回count个数据
 *  param:
 *      count:　返回数据的数量
 *  ret:　保存返回数据的链表
 */
std::list<std::string> REDISSTD::RedisSet::getRandMembers(uint32_t count)
{
    //检查该set,参数,命令执行器是否有效
    if (!m_valid || !m_redisManager.isValid())
    {
        return std::list<std::string>();
    }

    //返回链表
    std::list<std::string> valueList;

    char tmp[50] = {0};
    sprintf(tmp, "%d", count);

    //添加数据
    int32_t err = 0;
    std::string cmd = "SRANDMEMBER ";
    cmd.append(m_setName);
    cmd.append(" ");
    cmd.append(tmp);//SRANDMEMBER key [count]

    redisReply* reply = m_redisManager.execCmd(cmd, err);
    if (err == REDISACC::NO_ERR && reply != nullptr)//执行成功
    {
        redisReply* tmpReply = nullptr;
        for (int i = 0 ;i < reply->elements; i++)//拷贝所有的value值
        {
            tmpReply = reply->element[i];
            if (tmpReply != nullptr && (tmpReply->str != nullptr))
            {
                if (strcmp(SET_DEFAULT_VALUE, tmpReply->str))
                {
                    valueList.push_back(std::string(tmpReply->str, tmpReply->len));
                }
            }
            else
            {
                break;
            }
        }

        freeReplyObject(reply);
        reply = nullptr;
    }

    if (reply != nullptr)
    {
        freeReplyObject(reply);//释放
        reply = nullptr;
    }

    return valueList;
}
