#include "CSerialPortVirtual.h"
#include "CSerialPort/iutils.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <tchar.h> //_T
#else
#include <errno.h> // perror
#if defined(__APPLE__)
#include <util.h> // openpty
#else
#include <pty.h> // openpty
#endif

#include <stdio.h>      // printf
#include <sys/select.h> // select FD_ISSET FD_SET FD_ZERO
#include <unistd.h>     // read write

#include <thread> // std::thread
#include <condition_variable>
#include <chrono>

#define READ_DATA_MAX_LEN 4096

inline void handle_io(int src_fd, int dst_fd, char* src_name)
{
    char buffer[READ_DATA_MAX_LEN];
    int len = read(src_fd, buffer, sizeof(buffer));
    if (len > 0)
    {
        printf("%s receive [%d] %s\n", src_name, len, buffer);
        write(dst_fd, buffer, len);
    }
    else if (len == 0)
    {
        printf("port %s closed\n", src_name);
        close(src_fd);
    }
    else if (errno != EAGAIN)
    {
        perror("read error");
        close(src_fd);
    }
}
#endif

using namespace itas109;

CSerialPortVirtual::CSerialPortVirtual() {}

CSerialPortVirtual::~CSerialPortVirtual() {}

bool CSerialPortVirtual::createPair(char *portName1, char *portName2)
{
#ifdef _WIN32
    bool ret = false;
    HINSTANCE libInst;

    if (8 == sizeof(char *)) // 64bit application
    {
        libInst = LoadLibrary(_T("VSPDCTL64.DLL"));
    }
    else
    {
        libInst = LoadLibrary(_T("VSPDCTL.DLL"));
    }

    if (libInst)
    {
        typedef bool(__stdcall * CreatePairFn)(char *Port1, char *Port2);
        CreatePairFn CreatePair = (CreatePairFn)GetProcAddress(libInst, "CreatePair");
        if (CreatePair)
        {
            ret = CreatePair(portName1, portName2); // portName1 = "COM1", portName2 = "COM2"
        }
        FreeLibrary(libInst);
    }

    return ret;
#else
    std::condition_variable m_cv;
    std::mutex m_mutex;

    std::thread t([&]
        {
            int master1 = 0, slave1 = 0;
            int master2 = 0, slave2 = 0;
            char ptyName1[256] = {0};
            char ptyName2[256] = {0};

            if (openpty(&master1, &slave1, ptyName1, NULL, NULL) < 0 || openpty(&master2, &slave2, ptyName2, NULL, NULL) < 0)
            {
                perror("openpty failed\n");
                m_cv.notify_one();
                return;
            }

            itas109::IUtils::strncpy(portName1, ptyName1, 256);
            itas109::IUtils::strncpy(portName2, ptyName2, 256);
            printf("virtual serial port names: %s(%d) %s(%d)\n", portName1, master1, portName2, master2);
            m_cv.notify_one();

            fd_set readfds; // only care read fdset
            int max_fd = master1 > master2 ? master1 : master2;
            while (1)
            {
                // select will reset all params every time
                FD_ZERO(&readfds);         // clear all read fdset
                FD_SET(master1, &readfds); // add to read fdset
                FD_SET(master2, &readfds); // add to read fdset

                // -1 error, 0 timeout, >0 ok
                int ret = select(max_fd + 1, &readfds, NULL, NULL, NULL);
                if (ret < 0)
                {
                    if (errno == EINTR)
                    {
                        continue;
                    }
                    perror("select error");
                    break;
                }

                if (FD_ISSET(master1, &readfds))
                {
                    handle_io(master1, master2, ptyName1);
                }
                else if (FD_ISSET(master2, &readfds))
                {
                    handle_io(master2, master1, ptyName2);
                }
                else
                {
                }
            }

            close(master1);
            close(slave1);
            close(master2);
            close(slave2); 
        });
    t.detach();

    std::unique_lock<std::mutex> lock(m_mutex);
    if (!m_cv.wait_for(lock, std::chrono::microseconds(100), [&]{ return '\0' != portName1[0] && '\0' != portName2[0]; }))
    {
        printf("createPair wait timeout\n");
        return false;
    }

    return true;
#endif // _WIN32
}

bool itas109::CSerialPortVirtual::deletePair(char *portName)
{
#ifdef _WIN32
    bool ret = false;
    HINSTANCE libInst;

    if (8 == sizeof(char *)) // 64bit application
    {
        libInst = LoadLibrary(_T("VSPDCTL64.DLL"));
    }
    else
    {
        libInst = LoadLibrary(_T("VSPDCTL.DLL"));
    }

    if (libInst)
    {
        typedef bool(__stdcall * DeletePairFn)(char *Port1);
        DeletePairFn DeletePair = (DeletePairFn)GetProcAddress(libInst, "DeletePair");
        if (DeletePair)
        {
            ret = DeletePair(portName);
        }
        FreeLibrary(libInst);
    }

    return ret;
#else
    return true;
#endif
}
