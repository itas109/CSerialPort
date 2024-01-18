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

#if __cplusplus >= 201103L
// C++11 support: msvc2013 gcc4.8.5 clang3.0 etc.
#define CPP11_TIMER
#endif

#ifdef CPP11_TIMER
#include <chrono>
#include <condition_variable>
#endif

#include "iutils.hpp"
#include "ithread.hpp"

#define PORT_NAME_MAX_LEN 256

namespace itas109
{
template <class T> class ITimer
{
public:
    ITimer()
        : m_isRunning(false)
        , m_tryStop(false)
        , handle(I_THREAD_INITIALIZER)
    {
    }

    ~ITimer()
    {
        stop();
    }

    bool isRunning() const
    {
        return m_isRunning;
    }

#if defined(_WIN32)
    static unsigned int __stdcall threadFun(void *pParam)
#else
    static void *threadFun(void *pParam)
#endif
    {
        ITimer *p_base = (ITimer *)pParam;

#ifdef CPP11_TIMER
        std::mutex mutex;
        std::unique_lock<std::mutex> lock(mutex);
        if (!p_base->m_cv.wait_for(lock, std::chrono::milliseconds(p_base->m_timeoutMs), [&] { return p_base->m_tryStop; }))
        {
            // timeout
            ((p_base->p_class)->*(p_base->p_memfun))(p_base->m_portName, p_base->m_readBufferLen);
        }
#else
        itas109::IMutex mutex;
        if (p_base->m_cv.timeWait(mutex, p_base->m_timeoutMs, p_base->m_tryStop))
        {
            // timeout
            ((p_base->p_class)->*(p_base->p_memfun))(p_base->m_portName, p_base->m_readBufferLen);
        }
#endif

        // timer stoped or timeout => stop condition variable wake up
#ifdef CPP11_TIMER
        std::unique_lock<std::mutex> lockStop(p_base->m_stopMutex);
        p_base->m_isRunning = false;
        p_base->m_cvStop.notify_one();
#else
        IAutoLock lock(&p_base->m_stopMutex);
        p_base->m_isRunning = false;
        p_base->m_cvStop.notifyOne();
#endif

        return 0;
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

        m_isRunning = true;

        if (I_THREAD_INITIALIZER != handle)
        {
            i_thread_join(handle);
        }
        itas109::i_thread_create(&handle, NULL, threadFun, (void *)this);
    }

    void stop()
    {
        if (!m_isRunning || m_tryStop)
        {
            return;
        }

        m_tryStop = true;
#ifdef CPP11_TIMER
        m_cv.notify_one();
#else
        m_cv.notifyOne();
#endif

#ifdef CPP11_TIMER
        std::unique_lock<std::mutex> lock(m_stopMutex);
        m_cvStop.wait(lock, [this] { return m_isRunning == false; });
#else
        IAutoLock lock(&m_stopMutex);
        // TODO: fix this error
        // m_cvStop.wait(m_stopMutex, m_isRunning == false); // error
        while (m_isRunning == false)
        {
            m_cvStop.wait(m_stopMutex);
        }
#endif
        m_tryStop = false;
    }

private:
    volatile bool m_isRunning; // timer running status
    volatile bool m_tryStop;

#ifdef CPP11_TIMER
    std::condition_variable m_cvStop;
    std::condition_variable m_cv;
    std::mutex m_stopMutex;
#else
    itas109::IConditionVariable m_cvStop;
    itas109::IConditionVariable m_cv;
    itas109::IMutex m_stopMutex;
#endif

    itas109::i_thread_t handle;
    unsigned int m_timeoutMs;
    char m_portName[PORT_NAME_MAX_LEN];
    unsigned int m_readBufferLen;

    T *p_class;
    void (T::*p_memfun)(const char *, unsigned int);
};
} // namespace itas109
#endif // __I_TIMER_HPP__