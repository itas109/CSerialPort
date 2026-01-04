/**
 * @file ilockfile.hpp
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief lightweight cross-platform lock file library 轻量级跨平台锁文件类
 */
#ifndef __I_LOCKFILE_HPP__
#define __I_LOCKFILE_HPP__

#pragma once

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <string> // std::atoi

#if defined(_WIN32)
#define LOCKFILE_UNSUPPORTED
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#endif

namespace itas109
{
class ILockFile
{
public:
    ILockFile()
        : m_fd(-1)
    {
        m_path[0] = '\0';
    }

    ~ILockFile()
    {
        unlock();
    }

    bool lock(const char *deviceName)
    {
#ifdef LOCKFILE_UNSUPPORTED
        (void)deviceName;
        return false;
#else
        if (!buildLockPath(deviceName))
        {
            return false;
        }

        if (tryCreate())
        {
            return true;
        }

        if (errno == EEXIST)
        {
            if (checkStaleAndRecover())
            {
                return true;
            }
            return false;
        }

        if (errno == EACCES || errno == EPERM)
        {
            return false;
        }

        return false;
#endif
    }

    void unlock()
    {
#ifndef LOCKFILE_UNSUPPORTED
        if (m_fd >= 0)
        {
            close(m_fd);
            unlink(m_path);
            m_fd = -1;
        }
#endif
    }

    const char *path() const
    {
        return m_path;
    }

private:
#ifndef LOCKFILE_UNSUPPORTED
    bool buildLockPath(const char *deviceName)
    {
        const char *base = strrchr(deviceName, '/');
        base = base ? base + 1 : deviceName;

        const char *dirs[] = {
#if defined(__APPLE__)
            "/var/spool/uucp",
#elif defined(__linux__)
            "/run/lock",
            "/var/lock",
            "/var/spool/uucp",
#else
            "/var/spool/lock",
            "/var/spool/uucp",
#endif
            ".", // current directory
        };

        for (size_t i = 0; i < sizeof(dirs) / sizeof(dirs[0]); ++i)
        {
            if (access(dirs[i], W_OK) == 0)
            {
                snprintf(m_path, sizeof(m_path), "%s/LCK..%s", dirs[i], base);
                return true;
            }
        }
        return false;
    }

    bool tryCreate()
    {
        m_fd = open(m_path, O_CREAT | O_EXCL | O_WRONLY, 0644);
        if (m_fd < 0)
        {
            return false;
        }

        char buf[32];
        snprintf(buf, sizeof(buf), "%ld\n", (long)getpid());
        write(m_fd, buf, strlen(buf));
        return true;
    }

    bool checkStaleAndRecover()
    {
        // get pid from lock file
        int fd = open(m_path, O_RDONLY);
        if (fd < 0)
        {
            return false;
        }

        char buf[32] = {0};
        read(fd, buf, sizeof(buf) - 1);
        close(fd);

        pid_t pid = (pid_t)std::atoi(buf);
        if (pid <= 0)
        {
            return false;
        }

        // check if process is running
        if (kill(pid, 0) == -1 && errno == ESRCH)
        {
            // stale lock
            unlink(m_path);
            return tryCreate();
        }
        return false;
    }
#endif

private:
    char m_path[256];
    int m_fd;
};

} // namespace itas109
#endif // __I_LOCKFILE_HPP__
