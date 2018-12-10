#include "SerialPortInfoUnixBase.h"


CSerialPortInfoUnixBase::CSerialPortInfoUnixBase()
{
}


CSerialPortInfoUnixBase::~CSerialPortInfoUnixBase()
{
}

std::list<std::string> CSerialPortInfoUnixBase::availablePorts()
{
	std::list<std::string> portsList;
	return portsList;
}
