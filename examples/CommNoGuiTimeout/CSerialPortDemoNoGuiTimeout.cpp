#include <iostream>

#include "itimer.hpp"

#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"

#ifdef _WIN32
#include <windows.h>
#define imsleep(microsecond) Sleep(microsecond) // ms
#else
#include <unistd.h>
#define imsleep(microsecond) usleep(1000 * microsecond) // ms
#endif

#include <vector>
using namespace itas109;
using namespace std;

int countRead = 0;

class mySlot : public has_slots<>
{
public:
    mySlot(CSerialPort *sp)
    {
        recLen = -1;
        p_sp = sp;
    };

    void OnSendMessage()
    {
        if (timer.isRunning())
        {
            timer.stop();
        }

        timer.startOnce(50,
                        [this]
                        {
                            // read
                            recLen = p_sp->readAllData(str);

                            if (recLen > 0)
                            {
                                countRead++;

                                str[recLen] = '\0';
                                std::cout << "receive data : " << str << ", receive size : " << recLen << ", receive count : " << countRead << std::endl;
                            }
                        });
    };

private:
    mySlot(){};

private:
    CSerialPort *p_sp;

    char str[1024];
    int recLen;

    ITimer timer;
};

int main()
{
    size_t index = -1;
    std::string portName;
    vector<SerialPortInfo> m_availablePortsList;
    CSerialPort sp;

    std::cout << "Version : " << sp.getVersion() << std::endl << std::endl;

    mySlot receive(&sp);

    m_availablePortsList = CSerialPortInfo::availablePortInfos();

    std::cout << "availableFriendlyPorts : " << std::endl;

    for (size_t i = 0; i < m_availablePortsList.size(); i++)
    {
        std::cout << i << " - " << m_availablePortsList[i].portName << " " << m_availablePortsList[i].description << std::endl;
    }

    if (m_availablePortsList.size() == 0)
    {
        std::cout << "No valid port" << std::endl;
    }
    else
    {
        std::cout << std::endl;

        do
        {
            std::cout << "Please input index of the port(0 - " << (m_availablePortsList.size() - 1) << " ) : " << std::endl;

            std::cin >> index;

            if (index >= 0 && index < m_availablePortsList.size())
            {
                break;
            }
        } while (true);

        portName = m_availablePortsList[index].portName;
        std::cout << "select port name : " << portName << std::endl;

        sp.init(portName, // windows:COM1 Linux:/dev/ttyS0
                itas109::BaudRate9600, itas109::ParityNone, itas109::DataBits8, itas109::StopOne);

        sp.open();

        if (sp.isOpened())
        {
            std::cout << "open " << portName << " success" << std::endl;
        }
        else
        {
            std::cout << "open " << portName << " failed" << std::endl;
        }

        // connect for read
        sp.readReady.connect(&receive, &mySlot::OnSendMessage);

        // write
        sp.writeData("itas109", 7);

        for (;;)
        {
            imsleep(1);
        }
    }

    return 0;
}