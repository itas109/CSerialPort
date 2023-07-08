#include <iostream>

#ifdef _WIN32
#include <Windows.h>
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

class MyListener : public CSerialPortListener
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
                    std::cout << portName << " - Count: " << ++countRead << ", Length: " << recLen << ", Str: " << data << ", Hex: " << char2hexstr(data, recLen).c_str()
                              << std::endl;

                    // return receive data
                    p_sp->writeData(data, recLen);
                }

                delete[] data;
                data = NULL;
            }
        }
    };

private:
    CSerialPort *p_sp;
};

int main()
{
    CSerialPort sp;
    std::cout << "Version: " << sp.getVersion() << std::endl << std::endl;

    MyListener listener(&sp);

    std::vector<SerialPortInfo> m_availablePortsList = CSerialPortInfo::availablePortInfos();

    std::cout << "availableFriendlyPorts: " << std::endl;

    for (size_t i = 1; i <= m_availablePortsList.size(); ++i)
    {
        SerialPortInfo serialPortInfo = m_availablePortsList[i - 1];
        std::cout << i << " - " << serialPortInfo.portName << " " << serialPortInfo.description << " " << serialPortInfo.hardwareId << std::endl;
    }

    if (m_availablePortsList.size() == 0)
    {
        std::cout << "No valid port" << std::endl;
    }
    else
    {
        std::cout << std::endl;

        int input = -1;
        do
        {
            std::cout << "Please Input The Index Of Port(1 - " << m_availablePortsList.size() << ")" << std::endl;

            std::cin >> input;

            if (input >= 1 && input <= m_availablePortsList.size())
            {
                break;
            }
        } while (true);

        const char *portName = m_availablePortsList[input - 1].portName;
        std::cout << "Port Name: " << portName << std::endl;

        sp.init(portName,              // windows:COM1 Linux:/dev/ttyS0
                itas109::BaudRate9600, // baudrate
                itas109::ParityNone,   // parity
                itas109::DataBits8,    // data bit
                itas109::StopOne,      // stop bit
                itas109::FlowNone,     // flow
                4096                   // read buffer size
        );
        sp.setReadIntervalTimeout(0); // read interval timeout 0ms

        sp.open();
        std::cout << "Open " << portName << (sp.isOpen() ? " Success. " : " Failed. ");
        std::cout << "Code: " << sp.getLastError() << ", Message: " << sp.getLastErrorMsg() << std::endl;

        // connect for read
        sp.connectReadEvent(&listener);

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