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
#include <process.h>
#include <windows.h>
#else
#include <pthread.h>
#include <time.h> // clock_gettime
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

    ~IMutex()
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

    ~IMutex()
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
typedef ::pthread_t i_thread_t;

inline int i_thread_create(i_thread_t *thread, const pthread_attr_t *attr, void *(*thread_function)(void *), void *args)
{
    return ::pthread_create(thread, attr, thread_function, args);
}

inline void i_thread_join(i_thread_t thread)
{
    ::pthread_join(thread, 0);
}
#endif

#if defined(_WIN32)
typedef HANDLE i_condition_variable_t;

class IConditionVariable
{
public:
    IConditionVariable()
        : cond(INVALID_HANDLE_VALUE)
    {
        cond = CreateEvent(NULL,  // default security attributes
                           FALSE, // auto-reset event
                           FALSE, // initial state is nonsignaled
                           NULL   // object name
        );
    }

    ~IConditionVariable()
    {
        CloseHandle(cond);
    }

    virtual void wait(IMutex &mutex)
    {
        IAutoLock lock(&mutex);
        WaitForSingleObject(cond, INFINITE);
    };

    virtual int timeWait(IMutex &mutex, unsigned int timeoutMS)
    {
        IAutoLock lock(&mutex);
        DWORD ret = WaitForSingleObject(cond, timeoutMS);
        if (WAIT_TIMEOUT == ret)
        {
            return 1;
        }
        else
        {
            ResetEvent(cond);
            return 0;
        }
    };

    virtual void notifyOne()
    {
        SetEvent(cond);
    };

    virtual void notifyAll()
    {
        SetEvent(cond);
    };

private:
    i_condition_variable_t cond;
};
#else
typedef pthread_cond_t i_condition_variable_t;

class IConditionVariable
{
public:
    IConditionVariable()
    {
        pthread_cond_init(&cond, NULL);
    }

    ~IConditionVariable()
    {
        pthread_cond_destroy(&cond);
    }

    virtual void wait(IMutex &mutex)
    {
        IAutoLock lock(&mutex);
        i_mutex_t imutex = mutex.getLock();
        pthread_cond_wait(&cond, &imutex);
    };

    virtual int timeWait(IMutex &mutex, unsigned int timeoutMS)
    {
        IAutoLock lock(&mutex);
        timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += (time_t)(timeoutMS / 1000);
        abstime.tv_nsec += (timeoutMS % 1000) * 1000000;
        if (abstime.tv_nsec > 1000000000) // 1ms = 1000000ns
        {
            abstime.tv_sec += 1;
            abstime.tv_nsec -= 1000000000;
        }
        i_mutex_t imutex = mutex.getLock();
        int ret = pthread_cond_timedwait(&cond, &imutex, &abstime);
        if (ETIMEDOUT == ret)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    };

    virtual void notifyOne()
    {
        pthread_cond_signal(&cond);
    };

    virtual void notifyAll()
    {
        pthread_cond_broadcast(&cond);
    };

private:
    i_condition_variable_t cond;
};
#endif
} // namespace itas109
#endif // ifndef __I_THREAD_HPP__
