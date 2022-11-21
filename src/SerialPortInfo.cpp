#include "CSerialPort/SerialPortInfo.h"
#include "CSerialPort/SerialPort_global.h"

#ifdef I_OS_WIN
#include "CSerialPort/SerialPortInfoWinBase.h"
#define CSERIALPORTINFOBASE CSerialPortInfoWinBase
#elif defined I_OS_UNIX
#include "CSerialPort/SerialPortInfoUnixBase.h"
#define CSERIALPORTINFOBASE CSerialPortInfoUnixBase
#else
// Not support
#define CSERIALPORTBASE
#endif // I_OS_WIN

using namespace itas109;

CSerialPortInfo::CSerialPortInfo() {}

CSerialPortInfo::~CSerialPortInfo() {}

std::vector<itas109::SerialPortInfo> CSerialPortInfo::availablePortInfos()
{
    return CSERIALPORTINFOBASE::availablePortInfos();
}
