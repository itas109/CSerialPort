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

#if defined(_WIN32)
#include <windows.h>
#include <process.h> // _beginthreadex
#else
#include <pthread.h>
#include <time.h>  // clock_gettime
#include <errno.h> // ETIMEDOUT
#endif

namespace itas109
{
#if defined(_WIN32)
typedef CRITICAL_SECTION i_mutex_t;

class IMutex
{
public:
    IMutex()
    {
        InitializeCriticalSection(&m_mutex);
    }

    virtual ~IMutex()
    {
        DeleteCriticalSection(&m_mutex);
    }

    virtual void lock()
    {
        EnterCriticalSection(&m_mutex);
    }

    virtual void unlock()
    {
        LeaveCriticalSection(&m_mutex);
    }

    virtual i_mutex_t getLock()
    {
        return m_mutex;
    }

private:
    i_mutex_t m_mutex;
};
#else
typedef pthread_mutex_t i_mutex_t;

class IMutex
{
public:
    IMutex()
    {
        pthread_mutex_init(&m_mutex, NULL);
    }

    virtual ~IMutex()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    virtual void lock()
    {
        pthread_mutex_lock(&m_mutex);
    }

    virtual void unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }

    virtual i_mutex_t getLock()
    {
        return m_mutex;
    }

private:
    i_mutex_t m_mutex;
};
#endif

class IAutoLock
{
public:
    IAutoLock(IMutex *mutex)
        : m_mutex(mutex)
    {
        m_mutex->lock();
    }

    ~IAutoLock()
    {
        m_mutex->unlock();
    }

private:
    IMutex *m_mutex;
};

#if defined(_WIN32)
#define I_THREAD_INITIALIZER INVALID_HANDLE_VALUE
typedef HANDLE i_thread_t;

inline int i_thread_create(i_thread_t *thread, void const *, unsigned(__stdcall *thread_function)(void *), void *args)
{
    HANDLE h = (HANDLE)_beginthreadex(0, 0, thread_function, args, 0, 0);

    if (h != 0)
    {
        if (thread)
        {
            *thread = h;
        }
        return 0;
    }
    else
    {
        return -1;
    }
}

inline void i_thread_join(i_thread_t thread)
{
    ::WaitForSingleObject(thread, INFINITE);
    ::CloseHandle(thread);
}
#else
#define I_THREAD_INITIALIZER 0
typedef ::pthread_t i_thread_t;

inline int i_thread_create(i_thread_t *thread, const pthread_attr_t *attr, void *(*thread_function)(void *), void *args)
{
    return ::pthread_create(thread, attr, thread_function, args);
}

inline void i_thread_join(i_thread_t thread)
{
    ::pthread_join(thread, 0);
    thread = I_THREAD_INITIALIZER;
}
#endif

#if defined(_WIN32)
#define I_COND_INITIALIZER INVALID_HANDLE_VALUE
typedef HANDLE i_condition_variable_t;

class IConditionVariable
{
public:
    IConditionVariable()
        : cond(I_COND_INITIALIZER)
    {
        cond = ::CreateEvent(NULL,  // default security attributes
                             FALSE, // auto-reset event
                             FALSE, // initial state is nonsignaled
                             NULL   // object name
        );
    }

    ~IConditionVariable()
    {
        ::CloseHandle(cond);
    }

    virtual void wait(IMutex &mutex)
    {
        mutex.unlock();
        WaitForSingleObject(cond, INFINITE);
        mutex.lock();
    }

    virtual bool timeWait(IMutex &mutex, unsigned int timeoutMS)
    {
        mutex.unlock();
        DWORD ret = WaitForSingleObject(cond, timeoutMS);
        mutex.lock();
        if (WAIT_TIMEOUT == ret)
        {
            return true;
        }
        else
        {
            ResetEvent(cond);
            return false;
        }
    }

    virtual void wait(IMutex &mutex, volatile bool predicate)
    {
        while (!predicate)
        {
            wait(mutex);
        }
    }

    virtual bool timeWait(IMutex &mutex, unsigned int timeoutMS, volatile bool predicate)
    {
        while (!predicate)
        {
            if (timeWait(mutex, timeoutMS))
            {
                break;
            }
        }

        return !predicate;
    }

    virtual void notifyOne()
    {
        ::SetEvent(cond);
    }

    virtual void notifyAll()
    {
        ::SetEvent(cond);
    }

private:
    i_condition_variable_t cond;
};
#else
#define I_COND_INITIALIZER PTHREAD_COND_INITIALIZER
typedef pthread_cond_t i_condition_variable_t;

#ifndef __APPLE__
#define USE_MONOTONIC_CLOCK
#endif

class IConditionVariable
{
public:
    IConditionVariable()
    {
#ifdef USE_MONOTONIC_CLOCK
        pthread_condattr_t attr;
        ::pthread_condattr_init(&attr);
        ::pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
        ::pthread_cond_init(&cond, &attr);
        ::pthread_condattr_destroy(&attr);
#else
        ::pthread_cond_init(&cond, NULL);
#endif
    }

    ~IConditionVariable()
    {
        ::pthread_cond_destroy(&cond);
    }

    virtual void wait(IMutex &mutex)
    {
        i_mutex_t imutex = internal_mutex.getLock();
        mutex.unlock();
        ::pthread_cond_wait(&cond, &imutex);
        mutex.lock();
    }

    virtual bool timeWait(IMutex &mutex, unsigned int timeoutMS)
    {
        timespec abstime;
#ifdef USE_MONOTONIC_CLOCK
        // monotonic time is better
        clock_gettime(CLOCK_MONOTONIC, &abstime);
#else
        clock_gettime(CLOCK_REALTIME, &abstime);
#endif
        abstime.tv_sec += (time_t)(timeoutMS / 1000);
        abstime.tv_nsec += (timeoutMS % 1000) * 1000000;
        if (abstime.tv_nsec > 1000000000) // 1ms = 1000000ns
        {
            abstime.tv_sec += 1;
            abstime.tv_nsec -= 1000000000;
        }
        i_mutex_t imutex = internal_mutex.getLock();
        mutex.unlock();
        int ret = ::pthread_cond_timedwait(&cond, &imutex, &abstime);
        mutex.lock();
        if (ETIMEDOUT == ret)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    virtual void wait(IMutex &mutex, volatile bool predicate)
    {
        while (!predicate)
        {
            wait(mutex);
        }
    }

    virtual bool timeWait(IMutex &mutex, unsigned int timeoutMS, volatile bool predicate)
    {
        while (!predicate)
        {
            if (timeWait(mutex, timeoutMS))
            {
                break;
            }
        }

        return !predicate;
    }

    virtual void notifyOne()
    {
        IAutoLock lock(&internal_mutex);
        ::pthread_cond_signal(&cond);
    }

    virtual void notifyAll()
    {
        IAutoLock lock(&internal_mutex);
        ::pthread_cond_broadcast(&cond);
    }

private:
    i_condition_variable_t cond;
    IMutex internal_mutex;
};
#endif
} // namespace itas109
#endif // ifndef __I_THREAD_HPP__
