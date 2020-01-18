#include "SerialPort.h"

#ifdef I_OS_WIN
#include "SerialPortWinBase.h"
#define CSERIALPORTBASE CSerialPortWinBase
#elif defined I_OS_UNIX
#include "SerialPortUnixBase.h"
#define CSERIALPORTBASE CSerialPortUnixBase
#else
//Not support
#define CSERIALPORTBASE
#endif // I_OS_WIN

#include <iostream>

using namespace itas109;

CSerialPort::CSerialPort()
{
	p_serialPortBase = new CSERIALPORTBASE();

	p_serialPortBase->setMinByteReadNoify(1);

	((CSERIALPORTBASE *)p_serialPortBase)->readReady.connect(this, &CSerialPort::onReadReady);
}

itas109::CSerialPort::CSerialPort(const std::string & portName)
{
	p_serialPortBase = new CSERIALPORTBASE(portName);

    p_serialPortBase->setMinByteReadNoify(1);

    ((CSERIALPORTBASE *)p_serialPortBase)->readReady.connect(this, &CSerialPort::onReadReady);
}


CSerialPort::~CSerialPort()
{
    ((CSERIALPORTBASE *)p_serialPortBase)->readReady.disconnect_all();

	p_serialPortBase->~CSerialPortBase();
}

void itas109::CSerialPort::init(std::string portName, int baudRate /*= itas109::BaudRate::BaudRate9600*/, itas109::Parity parity /*= itas109::Parity::ParityNone*/, itas109::DataBits dataBits /*= itas109::DataBits::DataBits8*/, itas109::StopBits stopbits /*= itas109::StopBits::StopOne*/, itas109::FlowConctrol flowConctrol /*= itas109::FlowConctrol::FlowNone*/, int64 readBufferSize /*= 512*/)
{
	p_serialPortBase->init(portName, baudRate, parity, dataBits, stopbits, flowConctrol, readBufferSize);
}

void itas109::CSerialPort::init(int port, int baudRate /*= itas109::BaudRate9600*/, itas109::Parity parity /*= itas109::ParityNone*/, itas109::DataBits dataBits /*= itas109::DataBits8*/, itas109::StopBits stopbits /*= itas109::StopOne*/, itas109::FlowConctrol flowConctrol /*= itas109::FlowNone*/, int64 readBufferSize /*= 512*/)
{
    p_serialPortBase->init(port, baudRate, parity, dataBits, stopbits, flowConctrol, readBufferSize);
}

void CSerialPort::setOperateMode(OperateMode operateMode)
{
    p_serialPortBase->setOperateMode(operateMode);
}

bool itas109::CSerialPort::open()
{
    return p_serialPortBase->openPort();
}

void itas109::CSerialPort::close()
{
    p_serialPortBase->closePort();
}

bool itas109::CSerialPort::isOpened()
{
    return p_serialPortBase->isOpened();
}

int itas109::CSerialPort::readData(char *data, int maxSize)
{
	return p_serialPortBase->readData(data,maxSize);
}


int itas109::CSerialPort::readAllData(char *data)
{
	return p_serialPortBase->readAllData(data);
}

int itas109::CSerialPort::readLineData(char *data, int maxSize)
{
	return p_serialPortBase->readLineData(data,maxSize);
}

int itas109::CSerialPort::writeData(const char * data, int maxSize)
{
	return p_serialPortBase->writeData(data,maxSize);
}

void itas109::CSerialPort::setDebugModel(bool isDebug)
{
	p_serialPortBase->setDebugModel(isDebug);
}

void itas109::CSerialPort::setReadTimeInterval(int msecs)
{
	p_serialPortBase->setReadTimeInterval(msecs);
}

void CSerialPort::setMinByteReadNoify(unsigned int minByteReadNoify)
{
    p_serialPortBase->setMinByteReadNoify(minByteReadNoify);
}

int itas109::CSerialPort::getLastError() const
{
	return p_serialPortBase->getLastError();
}

void itas109::CSerialPort::clearError()
{
	p_serialPortBase->clearError();
}

void itas109::CSerialPort::setPortName(std::string portName)
{
	p_serialPortBase->setPortName(portName);
}

std::string itas109::CSerialPort::getPortName() const
{
	return p_serialPortBase->getPortName();
}

void itas109::CSerialPort::setBaudRate(int baudRate)
{
	p_serialPortBase->setBaudRate(baudRate);
}

int itas109::CSerialPort::getBaudRate() const
{
	return p_serialPortBase->getBaudRate();
}

void itas109::CSerialPort::setParity(itas109::Parity parity)
{
	p_serialPortBase->setParity(parity);
}

itas109::Parity itas109::CSerialPort::getParity() const
{
	return  p_serialPortBase->getParity();
}

void itas109::CSerialPort::setDataBits(itas109::DataBits dataBits)
{
	p_serialPortBase->setDataBits(dataBits);
}

itas109::DataBits itas109::CSerialPort::getDataBits() const
{
	return p_serialPortBase->getDataBits();
}

void itas109::CSerialPort::setStopBits(itas109::StopBits stopbits)
{
	p_serialPortBase->setStopBits(stopbits);
}

itas109::StopBits itas109::CSerialPort::getStopBits() const
{
	return p_serialPortBase->getStopBits();
}

void itas109::CSerialPort::setFlowConctrol(itas109::FlowConctrol flowConctrol)
{
	p_serialPortBase->setFlowConctrol(flowConctrol);
}

itas109::FlowConctrol itas109::CSerialPort::getFlowConctrol() const
{
	return p_serialPortBase->getFlowConctrol();
}

void itas109::CSerialPort::setReadBufferSize(int64 size)
{
	p_serialPortBase->setReadBufferSize(size);
}

int64 itas109::CSerialPort::getReadBufferSize() const
{
	return p_serialPortBase->getReadBufferSize();
}

void itas109::CSerialPort::setDtr(bool set /*= true*/)
{
	p_serialPortBase->setDtr(set);
}

void itas109::CSerialPort::setRts(bool set /*= true*/)
{
	p_serialPortBase->setRts(set);
}

std::string itas109::CSerialPort::getVersion()
{
    return "https://github.com/itas109/CSerialPort (itas109@qq.com) - CSerialPort V4.0.2.200108";
}

void itas109::CSerialPort::onReadReady()
{
    readReady._emit();
}
