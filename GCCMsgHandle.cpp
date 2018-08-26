//
// Created by chenbaiyi on 18-7-25.
//

#include "GCCMsgHandle.h"
#include "./tool/CDBBase.h"

#include <QtCore/QThread>

/*******************************************/
/**************GCCMsgHandleFactory**************/
/*******************************************/

GCCMsgHandleFactory::GCCMsgHandleFactory()
    : m_factoryThread(nullptr)
{
    //初始化日志模块
    if (m_msgHandleFactoryLogHandle.InitModule("MsgHandlFactoryLogMoudle"))
    {
        m_msgHandleFactoryLogHandle.WriteLog("MsgHandlFactoryLogMoudle init successful", CLog::DEBUG_LEVEL);
    }
    else
    {
        printf("MsgHandlFactoryLogMoudle init failed\n");
    }
    }
}

GCCMsgHandleFactory::~GCCMsgHandleFactory()
{
}

void GCCMsgHandleFactory::runFactory()
{
    //CTS CTC CTG三大类消息队列  ---每大类下边可能有n个细化的消息队列
    m_msgHandleList.push_back(new GCCMsgCTSHandle);
    (*m_msgHandleList.begin())->runHandle();
}

/*
 **@des:初始化线程
 **@param:
 *×@ret:
 **@date:
 */
void GCCMsgHandleFactory::initThread()
{
    if (m_factoryThread)
    {
        return;
    }

    m_factoryThread = new QThread;
    this->moveToThread(m_factoryThread);

    QObject::connect(m_factoryThread, &QThread::started, this, &GCCMsgHandleFactory::runFactory, Qt::QueuedConnection);

    QObject::connect(this, &GCCMsgHandleFactory::finished, [](){"msghandle factory quit...\n";});
    QObject::connect(this, &GCCMsgHandleFactory::finished, m_factoryThread, &QThread::quit);
    QObject::connect(this, &GCCMsgHandleFactory::finished, this, &GCCMsgHandleFactory::deleteLater);
    QObject::connect(m_factoryThread, &QThread::finished, m_factoryThread, &QThread::deleteLater);
}

/*
 **@des:开启线程
 **@param:
 *×@ret:
 *  false表示开启失败 true表示成功开启
 **@date:
 */
bool GCCMsgHandleFactory::start()
{
    if (m_factoryThread == nullptr)
    {
        return false;
    }

    m_factoryThread->start();
    return true;
}


/*******************************************/
/**************GCCMsgHandle**************/
/*******************************************/

GCCMsgHandle::GCCMsgHandle(uint8_t tpNum)
    : m_handleTP((!tpNum) ? (tpNum = 1) : (1)), m_dbManager(new CDBBase()) {
    m_redisManager.connectRedis(ConfigInfo::deployRedisIP, strtol(ConfigInfo::deployRedisPort.c_str(), nullptr, 0));

    //连接数据库
    if (m_dbManager.get()->InitDBBase())
    {
        StruConnectDBInfo connectInfo = {0};

        m_dbManager.get()->ConnectDB(connectInfo);
    }

    //初始化日志模块
    if (m_msgHandleLogHandle.InitModule("MsgHandlLogMoudle"))
    {
        m_msgHandleLogHandle.WriteLog("MsgHandlLogMoudle init successful", CLog::DEBUG_LEVEL);
    }
    else
    {
        printf("MsgHandlLogMoudle init failed\n");
    }
}

GCCMsgHandle::~GCCMsgHandle()
{
    //断开redis
    if (m_redisManager.isValid())
    {
        m_redisManager.disconnecRedis();
    }
}

void GCCMsgHandle::runHandle()
{

}


/*******************************************/
/**************GCCMsgCTSHandle**************/
/*******************************************/

GCCMsgCTSHandle::GCCMsgCTSHandle()
{

}

GCCMsgCTSHandle::~GCCMsgCTSHandle()
{

}

void GCCMsgCTSHandle::runHandle()
{
    m_msgHandleLogHandle.WriteLog("GCCmsgCTSHandle ....", CLog::DEBUG_LEVEL);

    //检查mysql和redis执行器是否都初始化成功；未成功则再次初始化，失败直接退出
    if (!m_redisManager.isValid() && !m_redisManager.connectRedis(ConfigInfo::deployRedisIP, strtol(ConfigInfo::deployRedisPort.c_str(), nullptr, 0)))
    {
        return;
    }

    //检查mysql是否连接正常
    if (!m_dbManager.get()->isValid())
    {
        StruConnectDBInfo connectInfo = {0};
        if (!m_dbManager.get()->InitDBBase() || !m_dbManager.get()->ConnectDB(connectInfo))
        {
            return ;
        }
    }

    //执行器检查正常之后开始派发任务

}



/*******************************************/
/**************GCCMsgCTCHandle**************/
/*******************************************/

GCCMsgCTCHandle::GCCMsgCTCHandle()
{

}

GCCMsgCTCHandle::~GCCMsgCTCHandle()
{

}

void GCCMsgCTCHandle::runHandle()
{
    m_msgHandleLogHandle.WriteLog("GCCmsgCTCHandle ....", CLog::DEBUG_LEVEL);
}


/*******************************************/
/**************GCCMsgCTGHandle**************/
/*******************************************/


GCCMsgCTGHandle::GCCMsgCTGHandle()
{

}

GCCMsgCTGHandle::~GCCMsgCTGHandle()
{

}

void GCCMsgCTGHandle::runHandle()
{
    m_msgHandleLogHandle.WriteLog("GCCmsgCTGHandle ....", CLog::DEBUG_LEVEL);


}

