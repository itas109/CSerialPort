#include "SerialPortInfo.h"

#ifdef I_OS_WIN
#include "SerialPortInfoWinBase.h"
#define CSERIALPORTINFOBASE CSerialPortInfoWinBase
#elif defined I_OS_UNIX
#include "SerialPortInfoUnixBase.h"
#define CSERIALPORTINFOBASE CSerialPortInfoUnixBase
#else
//Not support
#define CSERIALPORTBASE
#endif // I_OS_WIN

#include "SerialPortInfoBase.h"

using namespace itas109;

CSerialPortInfo::CSerialPortInfo()
{
	//p_serialPortInfoBase = NULL;
	//p_serialPortInfoBase = new CSERIALPORTINFOBASE();
}


CSerialPortInfo::~CSerialPortInfo()
{
	//if (p_serialPortInfoBase)
	//{
	//	delete p_serialPortInfoBase;
	//	p_serialPortInfoBase = NULL;
	//}
}

std::list<std::string> CSerialPortInfo::availablePorts()
{
	return CSERIALPORTINFOBASE::availablePorts();
}