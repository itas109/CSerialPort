#ifndef __CSERIALPORTINFOWINBASE_H__
#define __CSERIALPORTINFOWINBASE_H__

#include "SerialPortInfoBase.h"
class CSerialPortInfoWinBase :	public CSerialPortInfoBase
{
public:
	CSerialPortInfoWinBase();
	~CSerialPortInfoWinBase();

	static std::list<std::string> availablePorts();

private:
	bool getRegKeyValues(std::string regKeyPath, std::list<std::string> & portsList);

private:
	static CSerialPortInfoWinBase* p_serialPortInfoWinBase;
};
#endif//__CSERIALPORTINFOWINBASE_H__
