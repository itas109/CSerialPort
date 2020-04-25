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

using namespace itas109;

CSerialPortInfo::CSerialPortInfo()
{
}


CSerialPortInfo::~CSerialPortInfo()
{
}

std::list<std::string> CSerialPortInfo::availablePorts()
{
    return CSERIALPORTINFOBASE::availablePorts();
}

std::list<std::string> CSerialPortInfo::availableFriendlyPorts()
{
    return CSERIALPORTINFOBASE::availableFriendlyPorts();
}

vector<SerialPortInfo> CSerialPortInfo::availablePortInfos()
{
    return CSERIALPORTINFOBASE::availablePortInfos();
}