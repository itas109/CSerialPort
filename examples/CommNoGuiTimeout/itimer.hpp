#ifndef I_TIMER_H_
#define I_TIMER_H_

// C++11
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <thread>

class ITimer
{
public:
    ITimer() : m_isRunning(false), m_tryStop(false) {}

    ~ITimer()
    {
        stop();
    }

    bool isRunning() const
    {
        return m_isRunning;
    }

    void startOnce(int64_t timeoutMs, std::function<void()> task)
    {
        if (m_isRunning)
        {
            return;
        }

        m_isRunning = true;
        std::thread(
            [this, timeoutMs, task]()
            {
                std::mutex mutex;
                std::unique_lock<std::mutex> lock(mutex);
                if (!m_cv.wait_for(lock, std::chrono::milliseconds(timeoutMs), [this]() { return m_tryStop.load(); }))
                {
                    // timeout
                    task();
                }

                //{
                // timer stoped or timeout => stop condition variable wake up
                std::unique_lock<std::mutex> lockStop(m_stopMutex);
                m_isRunning = false;
                m_cvStop.notify_one();
                //}
            })
            .detach();
    }

    void stop()
    {
        if (!m_isRunning || m_tryStop)
        {
            return;
        }

        m_tryStop = true;
        m_cv.notify_one();

        std::unique_lock<std::mutex> lock(m_stopMutex);
        m_cvStop.wait(lock, [this] { return m_isRunning == false; });
        m_tryStop = false;
    }

private:
    std::atomic<bool> m_isRunning; // timer running status
    std::atomic<bool> m_tryStop;
    std::condition_variable m_cvStop;
    std::condition_variable m_cv;
    std::mutex m_stopMutex;
};
#endif // I_TIMER_H_