//
// Created by chenbaiyi on 18-4-25.
//

#ifndef TEST_JFCLOG_H
#define TEST_JFCLOG_H


#include <QtCore/QString>

class CLog
{
public:

    enum LOGLEVEL//输出级别
    {
        INFO_LEVEL,
        DEBUG_LEVEL,
        TRACE_LEVEL,
        ERROR_LEVEL,
        WARN_LEVEL,
        FATAL_LEVEL,
    };

    enum LOGMODE //输出模式
    {
        UNKNOW_MODE = 1,//未知模式
        SCRIPT_MODE = 2,//脚本配置
        CONSOLE_MODE = 4,//控制台模式
        FILE_MODE = 8,//文件模式---默认dailyrollingfileappender
        SOCKET_MODE = 16//重定向远程服务器
    };

    CLog();
    ~CLog();

public://供外部调用的具体函数
    bool InitModule(const char* s_moudle_name, unsigned int max_file_num = 30);    //通过输出模式初始化
    bool WriteLog(const char* log_buffer, LOGLEVEL log_level = DEBUG_LEVEL);//写入日志数据
    bool WriteLog(QString qs_log_data, LOGLEVEL log_level = DEBUG_LEVEL);//写入日志数据

    static CLog* getGlobalLogHandle();

private:
    bool Write(const char* log_buffer, LOGLEVEL& log_level);//实际写入的函数

    static CLog* gLogHandle;//用于全局日志输出

    //模式标识
    unsigned int m_log_mode;
    //日志实际写入操作句柄
    void* m_log_logger;
};


#endif //TEST_JFCLOG_H
