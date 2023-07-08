/*
 * Author : P.J. Kunst <kunst@prl.philips.nl>
 * Date   : 1993-02-25
 *
 * Purpose: This program demonstrates the use of the 'curses' library
 *          for the creation of (simple) menu-operated programs.
 *          In the PDCurses version, use is made of colors for the
 *          highlighting of subwindows (title bar, status bar etc).
 *
 * Acknowledgement: some ideas were borrowed from Mark Hessling's
 *                  version of the 'testcurs' program.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "tui.h"

#include <iostream>

// CSerialPort
#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
#include <vector>
using namespace itas109;

class mySlot : public CSerialPortListener
{
public:
	mySlot(CSerialPort& sp)
        : m_sp(sp)
    {
    };

	void onReadEvent(const char *portName, unsigned int readBufferLen)
	{
		if(readBufferLen > 0)
		{
			char *data = new char[readBufferLen + 1]; // '\0'

            if (data)
            {
                // read
                int recLen = m_sp.readData(data, readBufferLen);

                if (recLen > 0)
                {
                    data[recLen] = '\0';
                    
					bodymsg("[RX] - ");
					bodymsg(data);
					bodymsg("\n");
                }

                delete[] data;
                data = NULL;
            }
		}
	};

private:
	mySlot(){};

private:
	CSerialPort m_sp;
};

std::string ParityArray[] = {"None", "Odd", "Even", "Mark", "Space" };
std::string StopArray[] = {"1", "1.5", "2"};

std::string m_portName = "";
std::vector<SerialPortInfo> m_availablePortsList;
CSerialPort m_serialPort;
mySlot receive(m_serialPort);

/***************************** forward declarations ***********************/

void subSetting(void), subOpenClose(void), subSend(void), subReceive(void),subHelp(void);
void setPortName(void),setBaudRate(void),setParity(void),setDataBit(void),setStopBit(void);
void open(void), close(void);
void sendWindow(void),send(void);
void clearReceive(void);
void about(void);

/***************************** menus initialization ***********************/

menu MainMenu[] =
{
    { "Setting", subSetting, "Serial Port Setting" },
    { "OpenClose", subOpenClose, "Serial Port open and close" },// 打开 关闭
    { "Send", subSend, "Serial Port Send" },
    { "Receive", subReceive, "Serial Port Receive" },
    { "Help", subHelp, "Serial Port Help" },
    { "", (FUNC)0, "" }   // always add this as the last item!
};

menu SubMenuSetting[] =
{
    { "PortName", setPortName, "serial port name, such as COM1 or /dev/ttyS0" },
    { "BaudRate", setBaudRate, "baudrate, default 9600" },
    { "Parity", setParity, "parity, default None" },
    { "DataBit", setDataBit, "databit, default 8" },
    { "StopBit", setStopBit, "stopbit, default 1" },
    { "Exit", DoExit, "Exit CSerialPort TUI Demo" },
    { "", (FUNC)0, "" }
};

menu SubMenuOpenClose[] =
{
    { "Open", open, "open serial port" },
    { "Close", close, "close serial port" },
    { "", (FUNC)0, "" }
};

menu SubMenuSend[] =
{
    { "Send", send, "send some data to serial port" },
    { "", (FUNC)0, "" }
};

menu SubMenuReceive[] =
{
    { "Clear Receive", clearReceive, "clear receive" },
    { "", (FUNC)0, "" }
};

menu SubMenuHelp[] =
{
    { "About", about, "about" },
    { "", (FUNC)0, "" }
};

/***************************** main menu functions ************************/

void subSetting(void)
{
    domenu(SubMenuSetting);
}

void subOpenClose(void)
{
    domenu(SubMenuOpenClose);
}

void subSend(void)
{
    domenu(SubMenuSend);
}

void subReceive(void)
{
    domenu(SubMenuReceive);
}

void subHelp(void)
{
    domenu(SubMenuHelp);
}

/***************************** SubMenuSetting functions *************************/

void setPortName(void)
{
    m_availablePortsList = CSerialPortInfo::availablePortInfos();

    // std::cout << "availableFriendlyPorts : " << std::endl;

    bodymsg("availableFriendlyPorts : \n");

    for (int i = 0; i < m_availablePortsList.size(); i++)
	{
		// std::cout << i << " - " << m_availablePortsList[i].portName << " " << m_availablePortsList[i].description << std::endl;

        if( 0 == i)
        {
            m_portName = m_availablePortsList[i].portName;
        }

        char str[50] = {0};
        sprintf(str,"%d - %s %s\n",i,m_availablePortsList[i].portName, m_availablePortsList[i].description);
        bodymsg(str);
	}

    if (m_availablePortsList.size() == 0)
	{
        bodymsg("No valid portccess \n");
	}
}

void setBaudRate(void)
{
}

void setParity(void)
{
}

void setDataBit(void)
{
}

void setStopBit(void)
{

}

/***************************** SubMenuOpenClose functions *************************/

void open(void)
{
    // TODO: remove
    if(m_portName.empty())
    {
        setPortName();
    }

    char str[50] = {0};
    sprintf(str,"open %s\n",const_cast<char*>(m_portName.c_str()));
    bodymsg(str);

    m_serialPort.init(m_portName.c_str());//windows:COM1 Linux:/dev/ttyS0

	m_serialPort.open();
	
	if(m_serialPort.isOpen())
	{
        char str[50] = {0};
        sprintf(str,"open success. %s,%d,%s,%d,%s\n",
                const_cast<char*>(m_portName.c_str()),
                m_serialPort.getBaudRate(),
                const_cast<char*>(ParityArray[(int)(m_serialPort.getParity())].c_str()),
                (int)(m_serialPort.getDataBits()),
                const_cast<char*>(StopArray[(int)(m_serialPort.getStopBits())].c_str())
            );
        bodymsg(str);
		// std::cout << "open success" << std::endl;

        //connect for read
        m_serialPort.connectReadEvent(&receive);
	}
	else
	{
        char errorCodeStr[256];
        sprintf(errorCodeStr,"open failed, code : %d, message: %s\n",m_serialPort.getLastError(),m_serialPort.getLastErrorMsg()); 
        bodymsg(errorCodeStr);
	}
}

void close(void)
{
    bodymsg("close \n");
    m_serialPort.close();
}

/***************************** SubMenuSend functions *************************/

void send(void)
{
    if(m_serialPort.isOpen())
    {
        bodymsg("\n[TX] - itas109\n");
        //write
        m_serialPort.writeData("itas109", 7);

        /*
        char buffer[512];
        WINDOW *win = bodywin();
        //refresh();
        //wclear(win);
        echo();
        mvwaddstr(win, 10, 2, "Enter Send Data: ");
        wgetnstr(win, buffer, 512);
        m_serialPort.writeData(buffer, strlen(buffer));
        */
    }
    else
    {
        bodymsg("\nPlease open serial port first\n");
    }
    

}

/***************************** SubMenuReceive functions *************************/

void clearReceive(void)
{
    clsbody();
}

/***************************** SubMenuHelp functions *************************/

void about(void)
{
    bodymsg(const_cast<char*>(m_serialPort.getVersion()));
    bodymsg("\n");
}

/***************************** start main menu  ***************************/

int main(int argc, char **argv)
{
    setlocale(LC_ALL, "");

    startmenu(MainMenu, "CSerialPort Tui Demo - https://github.com/itas109/CSerialPort");

    return 0;
}
