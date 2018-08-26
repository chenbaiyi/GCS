//
// Created by chenbaiyi on 18-4-24.
//

#include "CDBBase.h"
#include "common_func.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <QtCore/QString>

//数据库监控函数
//void* cdbbase_thr_callback(void* arg)
//{
//    int* p_ret = nullptr;
//    JFCDBBase* p_mdb = JFCDBBase::get_instance();
//    JFCLog log;
//    log.InitModule("cdbbase_thr_callback");
//
//    //定时查询连接状态---如果断开连接则自动恢复
//    while (!p_mdb->b_thr_exit)
//    {
//        log.WriteLog("cdbbase_thr_callback called CheckDBState", JFCLog::DEBUG_LEVEL);
//        p_mdb->CheckDBState();
//
//        //睡5s
//        sleep(5);
//    }
//
//    return p_ret;
//}


/*******************************************/
/*******************************************/
/*******************************************/
/*******************************************/
/*******************************************/

std::string _tagConfigInfo::deployMachineID = "";//机器ID
std::string _tagConfigInfo::deployMachineIP = "";//绑定的ip地址
std::string _tagConfigInfo::deployMachinePort = "";//监听端口
std::string _tagConfigInfo::deployRedisIP = "";//redis的ip
std::string _tagConfigInfo::deployRedisPort = "";//redis的端口
std::string _tagConfigInfo::deployRedisUsername = "";//redis的账号
std::string _tagConfigInfo::deployRedisPwd = "";//redis的密码
std::string _tagConfigInfo::logPath = "";//日志路径



/*******************************************/
/*******************************************/
/***************CDBBase*******************/
/*******************************************/
/*******************************************/

//构造函数
CDBBase::CDBBase()
{
    //清空连接信息
    memset(&m_conn_info, 0, sizeof(m_conn_info));
    //清空管理数据库集合对象
    memset(&m_obj_info, 0, sizeof(m_obj_info));

    //初始化日志模块
    if (m_db_log.InitModule("DBLogMoudle"))
    {
        m_db_log.WriteLog("DBLogMoudle init successful", CLog::DEBUG_LEVEL);
    }
    else
    {
        printf("DBLogMoudle init failed\n");
    }

    //初始化互斥量
    m_db_mutex = PTHREAD_MUTEX_INITIALIZER;
}

CDBBase::~CDBBase()
{
    //销毁互斥量
    pthread_mutex_destroy(&m_db_mutex);
}


// purpose: 初始化数据库
// author: ChenRS
// datetime: 2018/4/26
// param:
// ret:
    //true表示初始化成功，false表示初始化失败
bool CDBBase::InitDBBase()
{
    pthread_mutex_lock(&m_db_mutex);

    bool b_ret = false;

    //是否已经进行了初始化
    if (m_obj_info.driver)
    {
        m_db_log.WriteLog("Already Init Driver", CLog::ERROR_LEVEL);
        //已经初始化---亦可标识初始化成功
        return true;
    }

    try
    {
        //获取驱动
        m_obj_info.driver = sql::mysql::get_driver_instance();

        //输出driver版本
        QString qsLog = QString("Mysql driver version: ") + QString::number(m_obj_info.driver->getMajorVersion());
        QByteArray byteLog = qsLog.toLocal8Bit();
        m_db_log.WriteLog(byteLog.data(), CLog::DEBUG_LEVEL);
        b_ret = true;
    }
    catch (sql::SQLException& e)//捕获数据库异常
    {
        std::string sLog = "Cannot get Mysql Driver: ";
        sLog.append(e.what());
        m_db_log.WriteLog(sLog.c_str(), CLog::ERROR_LEVEL);
    }

    pthread_mutex_unlock(&m_db_mutex);
    return b_ret;
}

// purpose: 连接数据库
// author: ChenRS
// datetime: 2018/4/26
// param:
    //conn_db_info: 具体连接参数
// ret:
    //true表示初始化成功，false表示初始化失败
bool CDBBase::ConnectDB(const StruConnectDBInfo& conn_db_info)
{
    pthread_mutex_lock(&m_db_mutex);

    bool b_ret = false;

    //检查是否获取了数据库驱动
    if (m_obj_info.driver == nullptr)
    {
        return b_ret;
    }

    //是否已经连接成功
    if (m_obj_info.conn)
    {
        m_db_log.WriteLog("DB already connect successful", CLog::ERROR_LEVEL);
        //已经初始化---亦可标识初始化成功
        return true;
    }

    //组装连接参数
    memset(&m_conn_info, 0, sizeof(m_conn_info));
    memcpy(m_conn_info.conn_user_name, conn_db_info.conn_user_name,GET_ARRAY_LEN(m_conn_info.conn_user_name));
    memcpy(m_conn_info.conn_password,conn_db_info.conn_password, GET_ARRAY_LEN(m_conn_info.conn_password));
    snprintf(m_conn_info.conn_host, GET_ARRAY_LEN(m_conn_info.conn_host), "tcp://%s:%d", conn_db_info.conn_host, conn_db_info.conn_port);
    memcpy(m_conn_info.conn_database,conn_db_info.conn_database, GET_ARRAY_LEN(m_conn_info.conn_database));

    //组装连接map参数
    sql::ConnectOptionsMap conn_properties;
    conn_properties["hostName"] = conn_db_info.conn_host;
    conn_properties["userName"] = conn_db_info.conn_user_name;
    conn_properties["password"] = conn_db_info.conn_password;
    conn_properties["schema"] = conn_db_info.conn_database;
    conn_properties["port"] = conn_db_info.conn_port;
    conn_properties["OPT_RECONNECT"] = true;

    try
    {
        //开始连接数据库
//        m_obj_info.conn = m_obj_info.driver->connect(m_conn_info.conn_host, m_conn_info.conn_user_name,
//                                                     m_conn_info.conn_password);
        //begin connect database
        m_obj_info.conn = m_obj_info.driver->connect(conn_properties);

        //自动提交置false
        m_obj_info.conn->setAutoCommit(false);

        //打开对应的数据库
        m_obj_info.conn->setSchema(m_conn_info.conn_database);
        //创建statement
        m_obj_info.state = m_obj_info.conn->createStatement();

        b_ret = true;
        m_db_log.WriteLog("Connect DB successful", CLog::DEBUG_LEVEL);
    }
    catch (sql::SQLException& e)
    {
        std::string sLog = "Connect mysql failed: ";
        sLog.append(e.what());
        m_db_log.WriteLog(sLog.c_str(), CLog::ERROR_LEVEL);
    }

    pthread_mutex_unlock(&m_db_mutex);
    return b_ret;
}

// purpose: 关闭数据库
// author: ChenRS
// datetime: 2018/4/26
// param:
    //s_sql: 执行的sql语句
    //p_result: 传出数据指针；
        //如果是查询sql，则返回nullptr表示执行失败，否则直接返回ResultSet对象，需要用户自己手动释放
        //如果是操作语句，直接返回nullptr
// ret:
    //返回true表示执行成功，false表示执行失败
bool CDBBase::ExecSql(const char* s_sql, int*& p_result)
{
    pthread_mutex_lock(&m_db_mutex);

    bool b_ret = false;

    //数据库是否已经连接成功
    if (s_sql == nullptr || m_obj_info.driver == nullptr ||
        m_obj_info.conn == nullptr || m_obj_info.state == nullptr)
    {
        pthread_mutex_unlock(&m_db_mutex);
        p_result = nullptr;
        return b_ret;
    }

    //调用execute函数；查询语句返回true，操作语句返回false
    try
    {
        b_ret = m_obj_info.state->execute(s_sql);
        if (!b_ret)//操作语句
        {
            b_ret = true;
            m_obj_info.conn->commit();
        }
        else//查询语句
        {
            //获取查询结果对象
            p_result = reinterpret_cast<int *>(m_obj_info.state->getResultSet());
        }
    }
    catch (sql::SQLException& e)
    {
        b_ret = false;
        p_result = nullptr;

        pthread_mutex_unlock(&m_db_mutex);

       m_db_log.WriteLog(e.what(), CLog::DEBUG_LEVEL);
    }

    pthread_mutex_unlock(&m_db_mutex);

    return b_ret;
}

// purpose: 关闭数据库
// author: ChenRS
// datetime: 2018/4/26
// param:
// ret:
void CDBBase::CloseDB()
{
    pthread_mutex_lock(&m_db_mutex);

    if (m_obj_info.state)
    {
        m_obj_info.state->close();
        delete m_obj_info.state;
        m_obj_info.state = nullptr;
    }

    if (m_obj_info.conn)
    {
        m_obj_info.conn->close();
        delete m_obj_info.conn;
        m_obj_info.conn = nullptr;
    }

    if (m_obj_info.driver)
    {
        m_obj_info.driver = nullptr;
    }

    m_db_log.WriteLog("Close DB", CLog::DEBUG_LEVEL);
    pthread_mutex_unlock(&m_db_mutex);
}

// purpose: 检查数据库状态---如果断开连接则自动重连
// author: ChenRS
// datetime: 2018/4/28
// param:
// ret:
void CDBBase::CheckDBState()
{
    if (!m_obj_info.driver || !m_obj_info.conn || !m_obj_info.state)
    {
        return;
    }

    //连接是否有效
    try
    {
        if (!m_obj_info.conn->isValid())//失去连接
        {
            m_db_log.WriteLog("DB Breakup, auto reconnect", CLog::DEBUG_LEVEL);

            pthread_mutex_lock(&m_db_mutex);

            //重连
            std::string s_log = "";
            if (m_obj_info.conn->reconnect())
            {
                s_log.append("DB reconnect successful");
            }
            else//重连失败
            {
                s_log.append("DB reconnect failed");
            }

            m_db_log.WriteLog(s_log.c_str(), CLog::DEBUG_LEVEL);

            pthread_mutex_unlock(&m_db_mutex);
        }
    }
    catch (sql::SQLException& e)
    {
        std::string sLog = "Connect mysql failed: ";
        sLog.append(e.what());
        m_db_log.WriteLog(sLog.c_str(), CLog::ERROR_LEVEL);
    }
}

bool CDBBase::isValid()
{
    bool b_ret = false;

    //检查是否获取了数据库驱动
    if (m_obj_info.driver == nullptr)
    {
        return b_ret;
    }

    //是否已经连接成功
    if (m_obj_info.conn && m_obj_info.conn->isValid())
    {
        return true;
    }
    else
    {
        return false;
    }
}










