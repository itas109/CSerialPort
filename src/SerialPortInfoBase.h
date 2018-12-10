#ifndef __CSERIALPORTINFOBASE_H__
#define __CSERIALPORTINFOBASE_H__

#include <list>

class CSerialPortInfoBase
{
public:
	CSerialPortInfoBase();
	~CSerialPortInfoBase();

	static std::list<std::string> availablePorts();
};
#endif//__CSERIALPORTINFOBASE_H__
