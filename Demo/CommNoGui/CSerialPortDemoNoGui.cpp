#include <iostream>

#include "sigslot.h"
#include "SerialPort.h"
#include "SerialPortInfo.h"

#include <vector>
#include <iterator>
using namespace itas109;
using namespace std;

int countRead = 0;

//For Linux
//g++ CSerialPortDemoNoGui.cpp SerialPort.cpp SerialPortBase.cpp SerialPortUnixBase.cpp -lpthread -o CSerialPortDemoNoGui
//g++ CSerialPortDemoNoGui.cpp SerialPortInfo.cpp SerialPortInfoBase.cpp SerialPortInfoUnixBase.cpp SerialPort.cpp SerialPortBase.cpp SerialPortUnixBase.cpp -lpthread -o CSerialPortDemoNoGui

class mySlot : public has_slots<>
{
public:
	mySlot(CSerialPort & sp){ m_sp = sp; };

	void OnSendMessage()
	{
		//read
		recLen = m_sp.readAllData(str);

		if(recLen > 0)
		{
			countRead++;

			str[recLen] = '\0';
			std::cout << "receive data : " << str << ", receive size : " << recLen << ", receive count : " << countRead << std::endl;

			if(countRead > 7)
			{
				std::cout << "close serial port when receive count > 7" << std::endl;
				m_sp.close();
			}
			else
			{		
				// return receive data
				m_sp.writeData(str, recLen);
			}
		}
	};

private:
	mySlot(){};

private:
	CSerialPort m_sp;

	char str[1024];
	int recLen;
};

int main()
{
	int index = -1;
	int i = 0;
	std::string portName = "";
	std::string friendlyPortName = "";
	std::list<std::string>::iterator it;
	std::list<std::string> m_availablePortsList;
	vector<std::string> m_availablePortsVector;
	CSerialPort sp;

	std::cout << "Version : " << sp.getVersion() << std::endl << std::endl;

	mySlot receive(sp);

	m_availablePortsList = CSerialPortInfo::availableFriendlyPorts();

	std::cout << "availableFriendlyPorts : " << std::endl;
	
	std::copy(m_availablePortsList.begin(), m_availablePortsList.end(), back_inserter(m_availablePortsVector));
	
	for (it = m_availablePortsList.begin(); it != m_availablePortsList.end(); it++)
	{
		std::cout << i++ << " - " << *it << std::endl;
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
			std::cout << "Please input index of the port(0 - " << i - 1 << " ) : " << std::endl;
		
			std::cin >> index;
			
			if(index >=0 && index < i)
			{
				break;
			}
		}while(true);
			
		std::string friendlyPortName = m_availablePortsVector[index];
		portName = friendlyPortName.substr(0, friendlyPortName.find(" "));
		std::cout << "select port name : " << friendlyPortName << std::endl;

		sp.init(portName);//windows:COM1 Linux:/dev/ttyS0

		sp.open();
		
		if(sp.isOpened())
		{
			std::cout << "open " << portName << " success" << std::endl;	
		}
		else
		{
			std::cout << "open " <<  portName << " failed" << std::endl;
		}		

		//connect for read
		sp.readReady.connect(&receive, &mySlot::OnSendMessage);

		//write
		sp.writeData("itas109", 7);

		while (true);
	}

	return 0;
}