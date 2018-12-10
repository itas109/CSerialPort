#ifndef __CSERIALPORTINFOUNIXBASE_H__
#define __CSERIALPORTINFOUNIXBASE_H__

#include "SerialPortInfoBase.h"
class CSerialPortInfoUnixBase :	public CSerialPortInfoBase
{
public:
	CSerialPortInfoUnixBase();
	~CSerialPortInfoUnixBase();

	static std::list<std::string> availablePorts();
};
#endif//__CSERIALPORTINFOUNIXBASE_H__
