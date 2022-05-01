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
#endif

namespace itas109
{
#if defined(_WIN32)
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

private:
    CRITICAL_SECTION m_mutex;
};
#else
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

private:
    pthread_mutex_t m_mutex;
};
#endif

class IAutoLock
{
public:
    IAutoLock(IMutex *mutex) : m_mutex(mutex)
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
        *thread = h;
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
} // namespace itas109
#endif // ifndef __I_THREAD_HPP__
