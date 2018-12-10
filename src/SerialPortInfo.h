#ifndef __CSERIALPORTINFO_H__
#define __CSERIALPORTINFO_H__

#include "SerialPort_global.h"

#include <list>

//class CSerialPortInfoBase;

namespace itas109
{
	class DLL_EXPORT CSerialPortInfo
	{
	public:
		CSerialPortInfo();
		~CSerialPortInfo();

		static std::list<std::string> availablePorts();

	//private:
		//CSerialPortInfoBase * p_serialPortInfoBase;
	};
}
#endif//__CSERIALPORTINFO_H__
