/**
 * @file ilog.hpp
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief lightweight cross-platform log library 轻量级跨平台日志类
 */
#ifndef __I_LOG_HPP__
#define __I_LOG_HPP__

#include <fstream> // open close
#include <ctime>   // time localtime

#if defined(_WIN32)
#include <windows.h> // FILETIME
#else
#include <sys/time.h> // gettimeofday
#endif

#include "iutils.hpp"

#ifdef _WIN32
#ifndef __func__
#define __func__ __FUNCTION__
#endif
#endif

#ifdef CSERIALPORT_DEBUG
#define LOG_TRACE(format, ...) itas109::ILog::getInstance().log(itas109::ILog::LOG_TRACE, __func__, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) itas109::ILog::getInstance().log(itas109::ILog::LOG_DEBUG, __func__, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) itas109::ILog::getInstance().log(itas109::ILog::LOG_INFO, __func__, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) itas109::ILog::getInstance().log(itas109::ILog::LOG_WARN, __func__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) itas109::ILog::getInstance().log(itas109::ILog::LOG_ERROR, __func__, format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...) itas109::ILog::getInstance().log(itas109::ILog::LOG_FATAL, __func__, format, ##__VA_ARGS__)
#define LOG_SET_LEVEL(level) itas109::ILog::getInstance().setLogLevel(level)
#else
#define LOG_TRACE(format, ...)
#define LOG_DEBUG(format, ...)
#define LOG_INFO(format, ...)
#define LOG_WARN(format, ...)
#define LOG_ERROR(format, ...)
#define LOG_FATAL(format, ...)
#define LOG_SET_LEVEL(level)
#endif

namespace itas109
{
/**
 * @brief Log class 日志类
 *
 */
class ILog
{
public:
    enum LogLevel
    {
        LOG_ALL = 0,
        LOG_TRACE = 1,
        LOG_DEBUG = 2,
        LOG_INFO = 3,
        LOG_WARN = 4,
        LOG_ERROR = 5,
        LOG_FATAL = 6,
        LOG_OFF = 7
    };

    /**
     * @brief get ILog instance 获取ILog实例
     *
     * @param prefix [in] log file prefix 日志文件前缀
     * @return return ILog instance 返回ILog实例
     */
    static ILog &getInstance(const char *prefix = "CSerialPort_")
    {
        static ILog instance(prefix);
        return instance;
    }

    /**
     * @brief set log level 设置日志级别
     *
     * @param level [in] log level 日志级别
     */
    void setLogLevel(LogLevel level)
    {
        m_minLevel = level;
    }

    void log(LogLevel level, const char *function, const char *message, ...)
    {
        if (m_logFile.is_open() && level >= m_minLevel)
        {
            va_list args;
            va_start(args, message);
            char formatMessage[1024];
            itas109::IUtils::strFormat(formatMessage, 1024, message, args);
            va_end(args);

            char currentTime[25];
            getCurrentDateTime(currentTime);
            const char *levelStr = getLogLevelString(level);
            char logMessage[2048];
            itas109::IUtils::strFormat(logMessage, 2048, "%s [%s] %s - %s\n", currentTime, levelStr, function, formatMessage);
            m_logFile << logMessage;
            m_logFile.flush();
            printf("%s", logMessage);
        }
    }

private:
    ILog(const char *prefix)
        : m_minLevel(LOG_INFO)
    {
        char fileName[40];
        itas109::IUtils::strncpy(fileName, prefix, 40);
        getCurrentDate(fileName + itas109::IUtils::strlen(prefix));
        itas109::IUtils::strncat(fileName, ".log", 45);
        m_logFile.open(fileName, std::ios::out | std::ios::app);
        if (!m_logFile.is_open())
        {
            fprintf(stderr, "could not open file %s\n", fileName);
        }
    }

    ~ILog()
    {
        if (m_logFile.is_open())
        {
            m_logFile.close();
        }
    }

private:
    std::ofstream m_logFile;
    LogLevel m_minLevel;

    void getCurrentDate(char *dateStr)
    {
        struct tm ltm;
        time_t now = time(0);
#if defined(_WIN32)
        localtime_s(&ltm, &now);
#else
        ltm = *localtime(&now);
#endif
        itas109::IUtils::strFormat(dateStr, 20, "%04d%02d%02d", 1900 + ltm.tm_year, 1 + ltm.tm_mon, ltm.tm_mday);
    }

    void getCurrentDateTime(char *dateTimeStr)
    {
        struct tm ltm;
        int msec = 0;
#if defined(_WIN32)
        FILETIME fileTime;
        GetSystemTimeAsFileTime(&fileTime);

        ULARGE_INTEGER uli;
        uli.LowPart = fileTime.dwLowDateTime;
        uli.HighPart = fileTime.dwHighDateTime;

        time_t time = (uli.QuadPart - 116444736000000000) / 10000000;
        localtime_s(&ltm, &time);

        msec = (uli.QuadPart % 10000000) / 10000;
#else
        struct timeval now;
        gettimeofday(&now, NULL);
        ltm = *localtime(&now.tv_sec);
        msec = now.tv_usec / 1000;
#endif
        itas109::IUtils::strFormat(dateTimeStr, 25, "%04d-%02d-%02d %02d:%02d:%02d.%03d", 1900 + ltm.tm_year, 1 + ltm.tm_mon, ltm.tm_mday, ltm.tm_hour, ltm.tm_min, ltm.tm_sec,
                                   msec);
    }

    const char *getLogLevelString(LogLevel level)
    {
        switch (level)
        {
            case LOG_ALL:
                return "ALL";
            case LOG_TRACE:
                return "TRACE";
            case LOG_DEBUG:
                return "DEBUG";
            case LOG_INFO:
                return "INFO";
            case LOG_WARN:
                return "WARN";
            case LOG_ERROR:
                return "ERROR";
            case LOG_FATAL:
                return "FATAL";
            case LOG_OFF:
                return "OFF";
            default:
                return "UNKNOWN";
        }
    }
};
} // namespace itas109
#endif // __I_LOG_HPP__
