//
// Created by chenbaiyi on 18-4-25.
//

#include "common_func.h"

#include "CLog.h"
#include "CDBBase.h"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <string>


//log4cplus模块引用的头文件
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/helpers/stringhelper.h>

//qt模块
#include <QtCore/QString>

CLog* CLog::gLogHandle = nullptr;

CLog* CLog::getGlobalLogHandle()
{
    if (gLogHandle == nullptr)
    {
        gLogHandle = new CLog;

        //初始化全局日志模块
        if (gLogHandle->InitModule("GlobalLogMoudle"))
        {
            //gLogHandle->WriteLog("NetCoreLogHandle init moudle successful", CLog::DEBUG_LEVEL);
            printf("GlobalLogHandle init moudle successful\n");
        }
        else
        {
            printf("GlobalLogHandle init moudle failed\n");
            delete gLogHandle;
            gLogHandle = nullptr;
        }
    }

    return gLogHandle;
}


//构造函数
CLog::CLog()
{
    //zero
    m_log_logger = nullptr;

    //初始化日志输出模式
    //检查是否存在jflog.properties文件存在---即通过脚本配置日志输出模式
    m_log_mode = UNKNOW_MODE;

    std::string s_fullpath = get_app_fullpath();
    std::string s_path_properties = s_fullpath + std::string("log.properties");
    std::string s_path_socket = s_fullpath + std::string("log.socket");
    std::string s_path_console = s_fullpath + std::string("log.console");
    std::string s_path_file = s_fullpath + std::string("log.file");

    if (access(s_path_properties.c_str(), F_OK) != -1)
    {
        m_log_mode = LOGMODE::SCRIPT_MODE;//脚本模式
    }
    else if (access(s_path_socket.c_str(), F_OK) != -1)//检查是否存在jflog.socket文件
    {
        m_log_mode = LOGMODE::SOCKET_MODE;
    }
    else
    {
        //控制台输出和文件输出两者可以并存
        if (access(s_path_console.c_str(), F_OK) != -1)//检查是否存在jflog.console文件
        {
            m_log_mode = LOGMODE::CONSOLE_MODE;
        }

        if (access(s_path_file.c_str(), F_OK) != -1)//检查是否存在jflog.file文件
        {
            m_log_mode = m_log_mode | LOGMODE::FILE_MODE;
        }
    }
}


CLog::~CLog()
{
    //delete
    if (m_log_logger != nullptr)
    {
        delete reinterpret_cast<log4cplus::Logger*>(m_log_logger);
        m_log_logger = nullptr;
    }

    //删除全局日志输出器
    if (gLogHandle != nullptr)
    {
        delete gLogHandle;
        gLogHandle = nullptr;
    }
}

// purpose: 初始化该模块
// param:
    //s_moudle_name: 模块名称
// ret:
    //true表示初始化成功，false表示初始化失败
bool CLog::InitModule(const char* s_moudle_name, unsigned int max_file_num)
{
    if (m_log_logger != nullptr)
    {
        return false;
    }

    bool b_ret = false;

    //未知模式
    if (!s_moudle_name || m_log_mode == LOGMODE::UNKNOW_MODE)
    {
        return b_ret;//初始化失败
    }

    //脚本模式
    if (m_log_mode == LOGMODE::SCRIPT_MODE)
    {
        try
        {
            //获取根节点
            m_log_logger = new log4cplus::Logger(log4cplus::Logger::getRoot());
            //加载脚本配置

            std::string s_path_properties = get_app_fullpath();
            s_path_properties.append("log.properties");
            log4cplus::PropertyConfigurator::doConfigure(s_path_properties.c_str());
        }
        catch (...)
        {
            printf("LOG InitModule Failed\n");
        }
    }
    else if (m_log_mode == LOGMODE::SOCKET_MODE)//远程重定向
    {

    }
    else
    {
        try
        {
            //通过logger.console获取句柄
            m_log_logger = new log4cplus::Logger(log4cplus::Logger::getInstance(s_moudle_name));

            //输出格式
            std::string pattern = "[";
            pattern.append(s_moudle_name);
            pattern.append("] %d{%Y/%m/%d %H:%M:%S.%q}  %X  [%m]%n");

            if ((m_log_mode & LOGMODE::FILE_MODE) == LOGMODE::FILE_MODE)
            {
                //输出格式
                std::auto_ptr<log4cplus::Layout> layout(new log4cplus::PatternLayout(pattern));

                std::string logPath = ConfigInfo::logPath;
                logPath.append("test.log");

                log4cplus::helpers::SharedObjectPtr<log4cplus::Appender> appender(
                        new log4cplus::DailyRollingFileAppender(logPath.c_str(), log4cplus::DAILY, true, max_file_num));
                appender->setName(LOG4CPLUS_TEXT("appener.file"));//设置名称

                //设置输出格式
                appender->setLayout(layout);

                //将构造的适配添加到logger句柄中---此处可添加多个
                ((log4cplus::Logger*)m_log_logger)->addAppender(appender);//
            }

            if ((m_log_mode & LOGMODE::CONSOLE_MODE) == LOGMODE::CONSOLE_MODE)
            {
                //输出格式
                std::auto_ptr<log4cplus::Layout> layout(new log4cplus::PatternLayout(pattern));

                log4cplus::helpers::SharedObjectPtr<log4cplus::Appender> appender(new log4cplus::ConsoleAppender());
                appender->setName("appener.console");//设置名称

                //设置输出格式
                appender->setLayout(layout);

                //将构造的适配添加到logger句柄中---此处可添加多个
                ((log4cplus::Logger*)m_log_logger)->addAppender(appender);//
            }

            //设置日志级别
            ((log4cplus::Logger*)m_log_logger)->setLogLevel(log4cplus::ALL_LOG_LEVEL);
        }
        catch (...) {
            printf("LOG InitModule Failed\n");
        }
    }

    b_ret = true;
    return b_ret;
}


// purpose: 写入日志数据
// param:
    //log_buffer: 写入数据buffer
    //log_level: 日志输出级别---初始化的时候默认拥有最高级别
// ret:
    //true表示写入成功，false表示写入失败
bool CLog::WriteLog(const char* log_buffer, LOGLEVEL log_level)
{
    //未进行初始化
    if (log_buffer == nullptr || m_log_logger == nullptr)
    {
        return false;
    }

    return Write(log_buffer, log_level);
}


// purpose: 写入日志数据
// param:
    //qs_log_data: 写入数据,针对于整形
    //log_level: 日志输出级别---初始化的时候默认拥有最高级别
// ret:
    //true表示写入成功，false表示写入失败
bool CLog::WriteLog(QString qs_log_data, CLog::LOGLEVEL log_level)
{
    //未进行初始化
    if (m_log_logger == nullptr)
    {
        return false;
    }

    QByteArray byteData = qs_log_data.toLocal8Bit();
    return Write(byteData.data(), log_level);
}

//写入日志泛接口
bool CLog::Write(const char* log_buffer, LOGLEVEL& log_level)
{
    bool b_ret = false;

    //根据级别输出日志
    switch ((int)log_level)
    {
        case INFO_LEVEL:
            LOG4CPLUS_INFO(*reinterpret_cast<log4cplus::Logger*>(m_log_logger), log_buffer);
            break;
        case DEBUG_LEVEL:
            LOG4CPLUS_DEBUG(*reinterpret_cast<log4cplus::Logger*>(m_log_logger), log_buffer);
            break;
        case TRACE_LEVEL:
            LOG4CPLUS_TRACE(*reinterpret_cast<log4cplus::Logger*>(m_log_logger), log_buffer);
            break;
        case ERROR_LEVEL:
            LOG4CPLUS_ERROR(*reinterpret_cast<log4cplus::Logger*>(m_log_logger), log_buffer);
            break;
        case WARN_LEVEL:
            LOG4CPLUS_WARN(*reinterpret_cast<log4cplus::Logger*>(m_log_logger), log_buffer);
            break;
        case FATAL_LEVEL:
            LOG4CPLUS_FATAL(*reinterpret_cast<log4cplus::Logger*>(m_log_logger), log_buffer);
            break;
        default:
            break;
    }

    b_ret = true;
    return b_ret;
}

