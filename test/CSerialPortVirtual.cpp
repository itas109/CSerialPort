#include "CSerialPortVirtual.h"
#include "CSerialPort/iutils.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <tchar.h> //_T
#else
#include <errno.h>      // perror
#include <pty.h>        // openpty
#include <stdio.h>      // printf
#include <sys/select.h> // select FD_ISSET FD_SET FD_ZERO
#include <unistd.h>     // read write

#define PORT_NAME_MAX_LEN 256
#define READ_DATA_MAX_LEN 4096
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
    int error = -1;

    int master1 = 0, slave1 = 0;
    int master2 = 0, slave2 = 0;
    char slaveName1[PORT_NAME_MAX_LEN] = {0};
    char slaveName2[PORT_NAME_MAX_LEN] = {0};

    error = openpty(&master1, &slave1, slaveName1, NULL, NULL);
    if (-1 == error)
    {
        perror("openpty failed\n");
        return -1;
    }

    error = openpty(&master2, &slave2, slaveName2, NULL, NULL);
    if (-1 == error)
    {
        perror("openpty failed\n");
        return -1;
    }

    printf("virtual serial port names: %s %s\n", slaveName1, slaveName2);

    itas109::IUtils::strncpy(portName1, slaveName1, 256);
    itas109::IUtils::strncpy(portName2, slaveName2, 256);

    fd_set readfds; // only care read fdset

    bool isMaster1 = false;
    int recLen = 0;
    char data[READ_DATA_MAX_LEN] = {0};

    while (1)
    {
        // select will reset all params every time
        FD_ZERO(&readfds);         // clear all read fdset
        FD_SET(master1, &readfds); // add to read fdset
        FD_SET(master2, &readfds); // add to read fdset
        // -1 error, 0 timeout, >0 ok
        error = select(master1 > master2 ? master1 + 1 : master2 + 1, &readfds, NULL, NULL, NULL);
        if (error < 0)
        {
            perror("select failed");
            break;
        }

        isMaster1 = FD_ISSET(master1, &readfds);
        recLen = read(isMaster1 ? master1 : master2, data, READ_DATA_MAX_LEN);
        if (recLen > 0)
        {
            printf("from %s - [%d] %s\n", isMaster1 ? slaveName1 : slaveName2, recLen, data);
            // send data to other pty
            write(isMaster1 ? master2 : master1, data, recLen);
        }
        else if (recLen == 0)
        {
            printf("%s closed\n", isMaster1 ? slaveName1 : slaveName2);
            break;
        }
        else
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                perror("read failed\n");
                break;
            }
        }
    }

    close(master1);
    close(slave1);
    close(master2);
    close(slave2);
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
