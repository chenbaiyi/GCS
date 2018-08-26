//
// Created by chenbaiyi on 18-7-22.
//

#ifndef GCMSG_REDISSTD_H
#define GCMSG_REDISSTD_H

#include "redisacc.h"

#include <stdint.h>
#include <string>
#include <list>
#include <vector>

#define SET_DEFAULT_VALUE   "default_set_value"
#define SET_DEFAULT_RET     "set_or_redismanager_invalid"

#define HASHMAP_DEFAULT_FIELD   "hashmap_default_field"
#define HASHMAP_DEFAULT_VALUE   "hashmap_default_value"
#define HASHMAP_DEFAULT_RET     "hashmap_or_redismanager_invalid"



namespace REDISSTD
{
    class RedisLocker
    {
    public:
        explicit RedisLocker(std::string lockerName, const uint32_t& existTime, REDISACC::RedisManager& redisManager);
        ~RedisLocker();

        bool tryLock();
        bool lock(uint32_t waitTime);
        void unLock();

    private:
        uint32_t m_lockTime;//锁定时间，用于保证超过有效时间不会再删除所
        uint32_t m_exsitTime;//分布式锁存在时间
        std::string m_lockerName;//锁定器名称
        REDISACC::RedisManager& m_redisManager;
    };

    class RedisSet
    {
    public:
        explicit RedisSet(std::string setName, REDISACC::RedisManager& redisManager);
        ~RedisSet();

        bool add(std::string& value);//添加单个数据
        bool remove(std::string& value);//移除单个数据
        std::list<std::string> allValues();//返回该集合所有成员
        std::list<std::string> getRandMembers(uint32_t count);//随机获取count个成员
        uint32_t size();//返回集合的大小
        bool isExist(std::string& value);//判断集合中是否存在某个值
        void empty();//清空集合
        bool isValid();//是否有效

    private:
        bool m_valid;
        std::string m_setName;

        REDISACC::RedisManager& m_redisManager;//负责具体redis命令的执行器
    };

    class RedisHashMap
    {
    public:
        explicit RedisHashMap(std::string mapName, REDISACC::RedisManager& redisManager, uint32_t lockTimeout= 0);

        ~RedisHashMap();

        bool insert(std::string& field, std::string&  value);
        bool insert(std::string&  field, std::stringstream& sstream);
        bool insertMulti(std::vector<std::string>& field, std::vector<std::string>& value);

        bool remove(std::string& field);
        std::list<std::string> allFields();
        std::list<std::string> allValues();
        uint32_t size();
        std::string find(std::string& field);
        void empty();
        bool isValid();

    private:
        bool m_valid;
        std::string m_hashmapName;
        REDISSTD::RedisLocker m_locker;//加锁器
        REDISACC::RedisManager& m_redisManager;//负责具体redis命令的执行器
    };
}




#endif //GCMSG_REDISSTD_H
