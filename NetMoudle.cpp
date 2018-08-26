#include "./tool/CDBBase.h"
#include "GCCMsgHandle.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QThread>


int main(int argc, char** argv)
{
    //创建qt程序对象
    QCoreApplication app(argc, argv);

    //初始化配置文件信息
    ConfigInfo::initConfigInfo();

    //创建消息处理器工厂对象---用于动态根据实时消息处理队列创建消息处理器
    std::shared_ptr<GCCMsgHandleFactory> gMsgHandleFactory(new GCCMsgHandleFactory);
    gMsgHandleFactory->initThread();
    //开启消息处理器工厂线程
    if (!gMsgHandleFactory->start())
    {
        CLog::getGlobalLogHandle()->WriteLog("MsgHandleFactory start thread failed\n", CLog::DEBUG_LEVEL);
        return 0;
    }


    return app.exec();
}