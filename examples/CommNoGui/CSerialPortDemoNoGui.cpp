#include <iostream>

#ifdef _WIN32
#include <windows.h>
#define imsleep(microsecond) Sleep(microsecond) // ms
#else
#include <unistd.h>
#define imsleep(microsecond) usleep(1000 * microsecond) // ms
#endif

#include <vector>

#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
using namespace itas109;

std::string char2hexstr(const char *str, int len)
{
    static const char hexTable[17] = "0123456789ABCDEF";

    std::string result;
    for (int i = 0; i < len; ++i)
    {
        result += "0x";
        result += hexTable[(unsigned char)str[i] / 16];
        result += hexTable[(unsigned char)str[i] % 16];
        result += " ";
    }
    return result;
}

int countRead = 0;

class MyListener : public CSerialPortListener, public CSerialPortHotPlugListener
{
public:
    MyListener(CSerialPort *sp)
        : p_sp(sp){};

    void onReadEvent(const char *portName, unsigned int readBufferLen)
    {
        if (readBufferLen > 0)
        {
            char *data = new char[readBufferLen + 1]; // '\0'

            if (data)
            {
                // read
                int recLen = p_sp->readData(data, readBufferLen);

                if (recLen > 0)
                {
                    data[recLen] = '\0';
                    printf("%s - Count: %d, Length: %d, Str: %s, Hex: %s\n", portName, ++countRead, recLen, data, char2hexstr(data, recLen).c_str());

                    // return receive data
                    p_sp->writeData(data, recLen);
                }

                delete[] data;
                data = NULL;
            }
        }
    };

    void onHotPlugEvent(const char *portName, int isAdd)
    {
        printf("portName: %s, isAdded: %d\n", portName, isAdd);
    }

private:
    CSerialPort *p_sp;
};

int main()
{
    CSerialPort sp;
    printf("Version: %s\n\n", sp.getVersion());

    MyListener listener(&sp);

    std::vector<SerialPortInfo> m_availablePortsList = CSerialPortInfo::availablePortInfos();

    printf("AvailableFriendlyPorts:\n");

    int availablePortCount = (int)m_availablePortsList.size();

    for (int i = 1; i <= availablePortCount; ++i)
    {
        SerialPortInfo serialPortInfo = m_availablePortsList[i - 1];
        printf("%d - %s %s %s\n", i, serialPortInfo.portName, serialPortInfo.description, serialPortInfo.hardwareId);
    }

    if (m_availablePortsList.size() == 0)
    {
        printf("No valid port\n");
    }
    else
    {
        std::cout << std::endl;

        int input = -1;
        do
        {
            printf("Please Input The Index Of Port(1 - %d)\n", availablePortCount);

            std::cin >> input;

            if (input >= 1 && input <= m_availablePortsList.size())
            {
                break;
            }
        } while (true);

        const char *portName = m_availablePortsList[input - 1].portName;
        printf("Port Name: %s\n", portName);

        sp.init(portName,              // windows:COM1 Linux:/dev/ttyS0
                itas109::BaudRate9600, // baudrate
                itas109::ParityNone,   // parity
                itas109::DataBits8,    // data bit
                itas109::StopOne,      // stop bit
                itas109::FlowNone,     // flow
                4096                   // read buffer size
        );
        sp.setReadIntervalTimeout(0);               // read interval timeout 0ms
        sp.setByteReadBufferFullNotify(4096 * 0.8); // buffer full notify

        sp.open();
        printf("Open %s %s\n", portName, sp.isOpen() ? "Success" : "Failed");
        printf("Code: %d, Message: %s\n", sp.getLastError(), sp.getLastErrorMsg());

        // connect for read
        sp.connectReadEvent(&listener);
        // connect for hot plug
        sp.connectHotPlugEvent(&listener);

        // write hex data
        char hex[5];
        hex[0] = 0x31;
        hex[1] = 0x32;
        hex[2] = 0x33;
        hex[3] = 0x34;
        hex[4] = 0x35;
        sp.writeData(hex, sizeof(hex));

        // write str data
        sp.writeData("itas109", 7);

        for (;;)
        {
            imsleep(1);
        }
    }

    return 0;
}