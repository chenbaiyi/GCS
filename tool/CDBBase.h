//
// Created by chenbaiyi on 18-4-24.
// 定义具体数据库管理控制的类
//

#ifndef JF_CDBBASE_H
#define JF_CDBBASE_H

#include "Config.h"
#include "CLog.h"
#include <pthread.h>

#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/statement.h>


/**************  数据结构定义  ***************/

//连接数据库结构定义
typedef struct _tagStruConnectDBInfo
{
    char conn_user_name[50];//用户名
    char conn_password[50];//密码
    char conn_host[50];//主机地址
    char conn_database[50];//数据库名
    int conn_port;//连接端口
}StruConnectDBInfo, *PStruConnectDBInfo;


//mysqlcppconn模型结构定义
typedef struct _tagStruMysqlCppConnObj
{
    sql::mysql::MySQL_Driver* driver;//mysql驱动---对应于机子上是否装有mysql服务
    sql::Connection* conn;//传入登录信息，连接对应的数据库
    sql::Statement* state;//数据库状态
}StruMysqlCppConnObj, *PStruMysqlCppConnObj;

/******************************************/


class CDBBase
{
public:
    CDBBase();
    ~CDBBase();

public://具体功能接口
    bool InitDBBase();//初始化数据库驱动
    bool ConnectDB(const StruConnectDBInfo& conn_db_info);//连接数据库
    bool ExecSql(const char* s_sql, int*& p_result);//执行sql
    void CloseDB();//关闭数据库
    void CheckDBState();//检查数据库状态---如果断开了连接自动重连
    bool isValid();//是否有效 有效返回true

private:
    StruConnectDBInfo m_conn_info;//保存的连接信息
    StruMysqlCppConnObj m_obj_info;//保存管理控制数据库对应的对象结构

    pthread_mutex_t m_db_mutex;//用于多线程同步
    CLog m_db_log;//用户输出日志信息
};



/********** 配置文件数据结构 [config.ini] **********/
typedef struct _tagConfigInfo
{
    static std::string deployMachineID;//机器ID
    static std::string deployMachineIP;//绑定的ip地址
    static std::string deployMachinePort;//监听端口

    static std::string deployRedisIP;//redis的ip
    static std::string deployRedisPort ;//redis的端口
    static std::string deployRedisUsername;//redis的账号
    static std::string deployRedisPwd;//redis的密码

    static std::string logPath;//日志路径

    static void initConfigInfo()
    {
        CConfig config;
        if (config.InitMoudle("./config.ini"))
        {
            _tagConfigInfo::deployMachineID = config.GetValue("MAIN", "machine_id").toStdString();
            _tagConfigInfo::deployMachineIP = config.GetValue("MAIN", "machine_ip").toStdString();
            _tagConfigInfo::deployMachinePort = config.GetValue("MAIN", "machine_port").toStdString();
            _tagConfigInfo::deployRedisIP = config.GetValue("REDIS", "redis_ip").toStdString();
            _tagConfigInfo::deployRedisPort = config.GetValue("REDIS", "redis_port").toStdString();
            _tagConfigInfo::deployRedisUsername = config.GetValue("REDIS", "redis_username").toStdString();
            _tagConfigInfo::deployRedisPwd = config.GetValue("REDIS", "redis_pwd").toStdString();
            _tagConfigInfo::logPath = config.GetValue("LOG", "path").toStdString();

        }
    }
}ConfigInfo, *PConfigInfo;


#endif //JF_CDBBASE_H
