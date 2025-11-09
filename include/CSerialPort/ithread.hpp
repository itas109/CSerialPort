/**
 * @file ithread.hpp
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief lightweight cross-platform thread library 轻量级跨平台线程类
 */
#ifndef __I_THREAD_HPP__
#define __I_THREAD_HPP__

#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace itas109
{
using i_mutex_t = std::mutex;

class IMutex
{
public:
    IMutex()
        : m_mutex()
    {
    }

    ~IMutex()
    {
    }

    void lock()
    {
        m_mutex.lock();
    }

    bool tryLock()
    {
        return m_mutex.try_lock();
    }

    void unlock()
    {
        m_mutex.unlock();
    }

    i_mutex_t &getMutex()
    {
        return m_mutex;
    }

private:
    i_mutex_t m_mutex;
};

class IScopedLock
{
public:
    explicit IScopedLock(IMutex &mutex)
        : m_mutex(mutex)
    {
        m_mutex.lock();
    }

    ~IScopedLock()
    {
        m_mutex.unlock();
    }

private:
    IMutex &m_mutex;
};
} // namespace itas109
#endif // ifndef __I_THREAD_HPP__
