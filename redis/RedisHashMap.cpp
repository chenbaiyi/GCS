//
// Created by chenbaiyi on 18-7-19.
//

#include "redisstd.h"
#include <sstream>
#include <vector>


/*
 *  des: 构造函数；加锁器默认有效60s
 *  param:
 *      mapName:　hashmap的名称　即对应redis内部的key
 *      redisManager:　redis命令执行器
 *      lockTimeout: 加锁超时时间--ms
 */
REDISSTD::RedisHashMap::RedisHashMap(std::string mapName, REDISACC::RedisManager& redisManager, uint32_t lockTimeout)
    : m_redisManager(redisManager), m_locker(std::string("REDISLOCKER:") + mapName, 0, redisManager)
{
    m_valid = false;//默认无效

    //hashmap的名称　即对应redis的key
    if (mapName.size() && m_redisManager.isValid())
    {
        //对该hashmap加锁 直到该对象被销毁
        if (!m_locker.lock(lockTimeout))//加锁失败
        {
            return;
        }


        //检查该hashmap的key值是否已经存在了
        int32_t err = 0;
        std::string cmd = "TYPE ";//type key
        cmd.append(mapName);

        redisReply* reply = redisManager.execCmd(cmd, err);
        if (err == REDISACC::NO_ERR && reply != nullptr && reply->str)//执行成功
        {
            //已存在并且该key值的类型为hashmap
            if (!strcmp("hash", reply->str))
            {
                m_valid = true;//标志置为true
                m_hashmapName = mapName;
            }
            else if (!strcmp("none", reply->str))//或者该key不存在 则新建一个并初始值为默认 [defalut_field defalt_value]
            {
                freeReplyObject(reply);
                reply = nullptr;
                cmd.empty();
                cmd = "HSET " + mapName + std::string(" ") + std::string(HASHMAP_DEFAULT_FIELD)
                      + std::string(" ") + std::string(HASHMAP_DEFAULT_VALUE);;
                reply = redisManager.execCmd(cmd, err);
                if (err == REDISACC::NO_ERR && reply != nullptr)//执行成功
                {
                    m_valid = true;//标志置为true
                    m_hashmapName = mapName;
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

REDISSTD::RedisHashMap::~RedisHashMap()
{

}

/*
 *  des: 插入数据
 *  param:
 *      field:　field值
 *      value:　value值
 *  ret:
 *      true标识插入成功，false标识插入失败
 */
bool REDISSTD::RedisHashMap::insert(std::string& field, std::string& value)
{
    //检查该hashmap,参数,命令执行器是否有效
    if (!m_valid || !field.size() || !m_redisManager.isValid())
    {
        return -1;
    }

    //添加数据
    int32_t err = 0;
    std::string cmd = "HSET ";
    cmd.append(m_hashmapName);
    cmd.append(" ");
    cmd.append(field);
    cmd.append(" ");
    cmd.append(value);//HSET key field value

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
 *  ＠des: 插入数据
 *  ＠param:
 *      field:　field值
 *      value:　stringstream类型的值
 *  ＠ret:
 *      true标识插入成功 false标识插入失败
 */
bool REDISSTD::RedisHashMap::insert(std::string& field, std::stringstream& sstream)
{
    //检查该hashmap,参数,命令执行器是否有效
    if (!m_valid || !field.size() || !m_redisManager.isValid())
    {
        return -1;
    }

    size_t sCmdLen[4];
    char* sCmd[4];
    std::string value = sstream.str();
    int index = 0;

    sCmdLen[index] = strlen("HSET");
    sCmd[index] = new char[sCmdLen[index]];
    memset(sCmd[index], 0, sCmdLen[index]);
    memcpy(sCmd[index], "HSET", sCmdLen[index]);
    index++;

    sCmdLen[index] = m_hashmapName.size();
    sCmd[index] = new char[sCmdLen[index]];
    memset(sCmd[index], 0, sCmdLen[index]);
    memcpy(sCmd[index], m_hashmapName.c_str(), sCmdLen[index]);
    index++;

    sCmdLen[index] = field.size();
    sCmd[index] = new char[sCmdLen[index]];
    memset(sCmd[index], 0, sCmdLen[index]);
    memcpy(sCmd[index], field.c_str(), sCmdLen[index]);
    index++;

    sCmdLen[index] = value.size();
    sCmd[index] = new char[sCmdLen[index]];
    memset(sCmd[index], 0, sCmdLen[index]);
    memcpy(sCmd[index], value.c_str(), sCmdLen[index]);

    int err = 0;
    redisReply* reply = m_redisManager.execCmdArgv(index + 1, const_cast<const char**>(&(sCmd[0])), &(sCmdLen[0]), err);

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
 *  ＠des: 插入多条数据
 *  ＠param:
 *      field:　field值
 *      value:　value值
 *  ＠ret:
 *      true标识插入成功 false标识插入失败
 */
bool REDISSTD::RedisHashMap::insertMulti(std::vector<std::string>& field, std::vector<std::string>& value)
{
//检查该hashmap,参数,命令执行器是否有效
    if (!m_valid || !field.size() || !value.size()
        || (field.size() != value.size()) || !m_redisManager.isValid())
    {
        return -1;
    }

    //申请保存传入数据的数组 并进行赋值
    std::vector<const char *> argv(field.size() + value.size() + 2);
    std::vector<size_t> argvlen(field.size() + value.size() + 2);
    int index = 0;

    static char hsetCmd[] = "HSET";
    argv[index] = hsetCmd;
    argvlen[index] = sizeof(hsetCmd)-1;
    ++index;
    argv[index] = m_hashmapName.c_str();
    argvlen[index] = m_hashmapName.size();
    ++index;

    for(int i = 0;i< field.size(); ++i)
    {
        argvlen[index] = field.size();
        argv[index] = new char[argvlen[index]];
        memset((void *) argv[index], 0, argvlen[index]);
        memcpy((void *) argv[index], field.data(), field.size());
        index++;

        argvlen[index] = value.size();
        argv[index] = new char[argvlen[index]];
        memset((void *) argv[index], 0, argvlen[index]);
        memcpy((void *) argv[index], value.data(), value.size());
        index++;
    }

    //执行命令
    int err = 0;
    redisReply* reply = m_redisManager.execCmdArgv(field.size() + value.size() + 2,&(argv[0]), &(argvlen[0]), err);

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
 *  des: 通过给定的field移除该键值对
 *  param:
 *      field:　field值
 *      value:　value值
 *  ret:
 *
 */
bool REDISSTD::RedisHashMap::remove(std::string& field)
{
    //检查该hashmap,参数,命令执行器是否有效
    if (!m_valid || !field.size() || !m_redisManager.isValid())
    {
        return false;
    }

    //添加数据
    int32_t err = 0;
    std::string cmd = "HDEL ";
    cmd.append(m_hashmapName);
    cmd.append(" ");
    cmd.append(field);//HDEL key field

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
 *  des: 获取hashmap的所有field值
 *  param:
 *  ret:　包含所有field的链表
 *
 */
std::list<std::string> REDISSTD::RedisHashMap::allFields()
{
    //检查该hashmap,参数,命令执行器是否有效
    if (!m_valid || !m_redisManager.isValid())
    {
        return std::list<std::string>();
    }

    //返回链表
    std::list<std::string> fieldList;

    //添加数据
    int32_t err = 0;
    std::string cmd = "HKEYS " + m_hashmapName;//HKEYS key
    redisReply* reply = m_redisManager.execCmd(cmd, err);
    if (err == REDISACC::NO_ERR && reply != nullptr)//执行成功
    {
        redisReply* tmpReply = nullptr;
        for (int i = 0 ;i < reply->elements; i++)//拷贝所有的field值
        {
            tmpReply = reply->element[i];
            //回复结构必须部位空并且回复结构里边的值不能为空指针
            if (tmpReply != nullptr && (tmpReply->str != nullptr))
            {
                if (strcmp(HASHMAP_DEFAULT_FIELD, tmpReply->str))
                {
                    fieldList.push_back(std::string(tmpReply->str, tmpReply->len));
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

    return fieldList;
}

/*
 *  des: 获取hashmap的所有value值
 *  param:
 *  ret:　包含所有value的链表
 *
 */
std::list<std::string> REDISSTD::RedisHashMap::allValues()
{
    //检查该hashmap,参数,命令执行器是否有效
    if (!m_valid || !m_redisManager.isValid())
    {
        return std::list<std::string>();
    }

    //返回链表
    std::list<std::string> valueList;

    //添加数据
    int32_t err = 0;
    std::string cmd = "HVALS " + m_hashmapName;//HVALS key
    redisReply* reply = m_redisManager.execCmd(cmd, err);
    if (err == REDISACC::NO_ERR && reply != nullptr)//执行成功
    {
        redisReply* tmpReply = nullptr;
        for (int i = 0 ;i < reply->elements; i++)//拷贝所有的value值
        {
            tmpReply = reply->element[i];
            if (tmpReply != nullptr && (tmpReply->str != nullptr))
            {
                if (strcmp(HASHMAP_DEFAULT_VALUE, tmpReply->str))
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
 *  des: 获取该hashmap的大小
 *  param:
 *  ret:　大小
 *
 */
uint32_t REDISSTD::RedisHashMap::size()
{
    //检查该hashmap,参数,命令执行器是否有效
    if (!m_valid || !m_redisManager.isValid())
    {
        return false;
    }

    int size = 0;

    //添加数据
    int32_t err = 0;
    std::string cmd = "HLEN " + m_hashmapName;//HLEN key
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
 *  des: 通过给定field获取对应的value
 *  param:
 *      field: field值
 *  ret:　对应field的value值
 */
std::string REDISSTD::RedisHashMap::find(std::string& field)
{
    //检查该hashmap,参数,命令执行器是否有效
    if (!m_valid || !m_redisManager.isValid() || !field.size())
    {
        return HASHMAP_DEFAULT_RET;
    }

    std::string value = "";

    //添加数据
    int32_t err = 0;
    std::string cmd = "HGET ";
    cmd.append(m_hashmapName);
    cmd.append(" ");
    cmd.append(field);//HGET key field

    redisReply* reply = m_redisManager.execCmd(cmd, err);
    if (err == REDISACC::NO_ERR && reply != nullptr)//执行成功
    {
        if (reply->str)
        {
            value = std::string(reply->str, reply->len);
        }
        freeReplyObject(reply);
        reply = nullptr;
    }

    if (reply != nullptr)
    {
        freeReplyObject(reply);//释放
        reply = nullptr;
    }

    return value;
}

/*
 *  des: 清空hashmap的所有元素　但是必须保留默认的键值对　否则该hashmap就会被清除掉
 *  param:
 *  ret:　
 */
void REDISSTD::RedisHashMap::empty()
{
    //获取所有的field
    std::list<std::string> fieldList = allFields();

    //添加数据
    int32_t err = 0;
    std::string cmd = "DEL ";
    cmd.append(m_hashmapName);
    redisReply* reply = m_redisManager.execCmd(cmd, err);
    if (reply != nullptr)
    {
        freeReplyObject(reply);
        reply = nullptr;
    }
}

/*
 *  des: 检查该hashmap是否有效
 *  param:
 *  ret:　
 *      true标识有效　false标识无效
 */
bool REDISSTD::RedisHashMap::isValid()
{
    return m_valid;
}


