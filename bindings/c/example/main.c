/**
 * @file main.c
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief C CSerialPort Example  C的CSerialPort示例程序
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "cserialport.h"

int countRead = 0;

void char2hexstr(char *dest, const char *src, int len)
{
    static const char hexTable[17] = "0123456789ABCDEF";

    for (int i = 0; i < len; ++i)
    {
        // 0x + two bit hex +  one bit space => 5 bit
        dest[5 * i + 0] = '0';
        dest[5 * i + 1] = 'x';
        dest[5 * i + 2] = hexTable[(unsigned char)src[i] / 16];
        dest[5 * i + 3] = hexTable[(unsigned char)src[i] % 16];
        dest[5 * i + 4] = ' ';
    }
    dest[5 * len] = '\0';
}

void onReadEvent(void *pSerialPort, const char *portName, unsigned int readBufferLen)
{
    if (readBufferLen > 0)
    {
        char *data = malloc(readBufferLen + 1); // '\0'

        if (data)
        {
            // read
            int recLen = CSerialPortReadData(pSerialPort, data, readBufferLen);

            if (recLen > 0)
            {
                data[recLen] = '\0';

                char *hexStr = malloc(5 * recLen + 1);

                char2hexstr(hexStr, data, recLen);
                printf("%s - Count: %d Length: %d, Str: %s, Hex: %s\n", portName, ++countRead, recLen, data, hexStr);

                if (hexStr)
                {
                    free(hexStr);
                    hexStr = NULL;
                }

                // return receive data
                CSerialPortWriteData(pSerialPort, data, recLen);
            }

            free(data);
            data = NULL;
        }
    }
}

int main()
{
    void *pSerialPort = NULL;
    pSerialPort = CSerialPortMalloc();
    printf("Version: %s\n\n", CSerialPortGetVersion(pSerialPort));

    printf("Available Friendly Ports:\n");

    struct SerialPortInfoArray portInfoArray = {0};
    CSerialPortAvailablePortInfos(&portInfoArray);

    for (unsigned int i = 0; i < portInfoArray.size; ++i)
    {
        printf("%u - %s %s\n", i + 1, portInfoArray.portInfo[i].portName, portInfoArray.portInfo[i].description);
    }

    if (portInfoArray.size == 0)
    {
        printf("No Valid Port\n");
    }
    else
    {
        printf("\n");

        unsigned int input = 0;
        do
        {
            printf("Please Input The Index Of Port(1 - %d)\n", portInfoArray.size);

            scanf("%u", &input);

            if (input >= 1 && input <= portInfoArray.size)
            {
                break;
            }
        } while (1);

        char portName[256] = {0};
        strcpy(portName, portInfoArray.portInfo[input - 1].portName);
        printf("Port Name: %s\n", portName);

        CSerialPortAvailablePortInfosFree(&portInfoArray);

        CSerialPortInit(pSerialPort,
                        portName,   // windows:COM1 Linux:/dev/ttyS0
                        9600,       // baudrate
                        ParityNone, // parity
                        DataBits8,  // data bit
                        StopOne,    // stop bit
                        FlowNone,   // flow
                        4096        // read buffer size
        );
        CSerialPortSetReadIntervalTimeout(pSerialPort, 0); // read interval timeout

        CSerialPortOpen(pSerialPort);

        printf("Open %s %s\n", portName, 1 == CSerialPortIsOpen(pSerialPort) ? "Success" : "Failed");
        printf("Code: %d, Message: %s\n", CSerialPortGetLastError(pSerialPort), CSerialPortGetLastErrorMsg(pSerialPort));

        // connect for read
        CSerialPortConnectReadEvent(pSerialPort, onReadEvent);

        // write hex data
        char hex[5];
        hex[0] = 0x31;
        hex[1] = 0x32;
        hex[2] = 0x33;
        hex[3] = 0x34;
        hex[4] = 0x35;
        CSerialPortWriteData(pSerialPort, hex, sizeof(hex));

        // write str data
        CSerialPortWriteData(pSerialPort, "itas109", 7);
    }

    for (;;)
    {
    }

    CSerialPortDisconnectReadEvent(pSerialPort);

    CSerialPortFree(pSerialPort);

    return 0;
}