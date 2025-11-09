/**
 * @file itimer.hpp
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief lightweight cross-platform timer library 轻量级跨平台定时器类
 */
#ifndef __I_TIMER_HPP__
#define __I_TIMER_HPP__

// #if __cplusplus >= 201103L || _MSVC_LANG >= 201103L
// // C++11 support: msvc2013 gcc4.8.5 clang3.0 etc.
// #define CPP11_TIMER
// #endif

#include <chrono>
#include <condition_variable>
#include <thread>
#include <atomic>

#include "iutils.hpp"
#include "ithread.hpp"

#ifdef CSERIALPORT_DEBUG
#include "ilog.hpp"
#endif

#define PORT_NAME_MAX_LEN 256

namespace itas109
{
template <class T>
class ITimer
{
public:
    ITimer() {};

    ~ITimer()
    {
        stop();
    }

    bool isRunning() const
    {
        return m_isRunning;
    }

    static void threadFun(ITimer *p_base)
    {

        std::mutex mutex;
        std::unique_lock<std::mutex> lock(mutex);
        if (!p_base->m_cv.wait_for(lock, std::chrono::milliseconds(p_base->m_timeoutMs), [&]
                                   { return p_base->m_tryStop.load(std::memory_order_acquire); }))
        {
            // timeout
#ifdef CSERIALPORT_DEBUG
            LOG_INFO("onReadEvent timeout. portName: %s, readLen: %u", p_base->m_portName, p_base->m_readBufferLen);
#endif
            ((p_base->p_class)->*(p_base->p_memfun))(p_base->m_portName, p_base->m_readBufferLen);
        }

        // timer stoped or timeout => stop condition variable wake up
        std::unique_lock<std::mutex> lockStop(p_base->m_stopMutex);
        p_base->m_isRunning.store(false, std::memory_order_release);
        p_base->m_cvStop.notify_one();
    }

    void startOnce(unsigned int timeoutMs, T *pclass, void (T::*pmemfun)(const char *, unsigned int), const char *portName, unsigned int readBufferLen)
    {
        m_timeoutMs = timeoutMs;
        p_class = pclass;
        p_memfun = pmemfun;
        itas109::IUtils::strncpy(m_portName, portName, PORT_NAME_MAX_LEN);
        m_readBufferLen = readBufferLen;

        if (m_isRunning)
        {
            return;
        }

        m_isRunning.store(true, std::memory_order_release);

        if (m_thread.joinable())
        {
            m_thread.join();
        }
        m_thread = std::thread(&ITimer::threadFun, this);
    }

    void stop()
    {
        if (!m_isRunning || m_tryStop)
        {
            return;
        }

        m_tryStop.store(true, std::memory_order_release);
        m_cv.notify_one();

        std::unique_lock<std::mutex> lock(m_stopMutex);
        m_cvStop.wait(lock, [this]
                      { return m_isRunning.load(std::memory_order_acquire) == false; });
        m_tryStop.store(false, std::memory_order_release);
    }

private:
    std::atomic<bool> m_isRunning{false}; // timer running status
    std::atomic<bool> m_tryStop{false};   // timer try to stop

    std::condition_variable m_cvStop;
    std::condition_variable m_cv;
    std::mutex m_stopMutex;

    std::thread m_thread;

    unsigned int m_timeoutMs;
    char m_portName[PORT_NAME_MAX_LEN];
    unsigned int m_readBufferLen;

    T *p_class;
    void (T::*p_memfun)(const char *, unsigned int);
};
} // namespace itas109
#endif // __I_TIMER_HPP__