//
// Created by chenbaiyi on 18-7-25.
//

#ifndef GCMSG_GCCMSGHANDLE_H
#define GCMSG_GCCMSGHANDLE_H

#include "./tool/CLog.h"
#include "./tool/CDBBase.h"
#include "./redis/redisacc.h"

#include <boost/threadpool.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <QtCore/qobject.h>



/*
**@des:消息处理器基类
**@author:chenrs
**@date:2018/7/29
*/
class GCCMsgHandle
{
public:
    explicit GCCMsgHandle(uint8_t tpNum = 10);
    virtual ~GCCMsgHandle();

    virtual void runHandle();

protected:
    boost::threadpool::pool m_handleTP;//线程管理
    typedef boost::singleton_pool<struct pool_tag, sizeof(int)> m_handleSP;//消息处理用内存池
    REDISACC::RedisManager m_redisManager;//redis命令执行器
    std::unique_ptr<CDBBase> m_dbManager;//mysql执行器
    CLog m_msgHandleLogHandle;//消息处理器日志句柄
};


/*******************************************/
/**************GCCMsgCTSHandle**************/
/*******************************************/

/*
**@des:用于处理类型是CTS的类
**@author:chenrs
**@date:2018/7/29
*/
class GCCMsgCTSHandle : public GCCMsgHandle
{
public:
    explicit GCCMsgCTSHandle();
    virtual ~GCCMsgCTSHandle();

    virtual void runHandle();
};


/*******************************************/
/**************GCCMsgCTCHandle**************/
/*******************************************/

/*
**@des:用于处理类型是CTC的类
**@author:chenrs
**@date:2018/7/29
*/
class GCCMsgCTCHandle : public GCCMsgHandle
{
public:
    explicit GCCMsgCTCHandle();
    virtual ~GCCMsgCTCHandle();

    virtual void runHandle();
};


/*******************************************/
/**************GCCMsgCTGHandle**************/
/*******************************************/

/*
**@des:用于处理类型是CTG的类
**@author:chenrs
**@date:2018/7/29
*/
class GCCMsgCTGHandle : public GCCMsgHandle
{
public:
    explicit GCCMsgCTGHandle();
    virtual ~GCCMsgCTGHandle();

    virtual void runHandle();
};


/*******************************************/
/**************GCCMsgHandleFactory**************/
/*******************************************/

/*
**@des:创建消息处理器的的工厂类
**@author:chenrs
**@date:2018/7/29
*/

class GCCMsgHandleFactory : public QObject
{
    Q_OBJECT

public:
    explicit GCCMsgHandleFactory();
    ~GCCMsgHandleFactory();

    void runFactory();//运行消息处理器工厂
    void initThread();
    bool start();

signals:
    void finished();

private:
    CLog m_msgHandleFactoryLogHandle;//消息处理器工厂日志句柄
    std::list<GCCMsgHandle*> m_msgHandleList;//消息处理器工厂创建的消息处理器链表
    QThread* m_factoryThread;
};




















#endif //GCMSG_GCCMSGHANDLE_H
