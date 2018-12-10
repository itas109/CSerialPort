#include "SerialPort.h"

//#include "osplatformutil.h"
//
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

//sigslot::signal0<> CSerialPort::readReady;//sigslot

CSerialPort::CSerialPort()
{
	p_serialPortBase = new CSERIALPORTBASE();

	((CSERIALPORTBASE *)p_serialPortBase)->readReady.connect(this, &CSerialPort::onReadReady);
}

itas109::CSerialPort::CSerialPort(const std::string & portName)
{
	p_serialPortBase = new CSERIALPORTBASE(portName);
}


CSerialPort::~CSerialPort()
{
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

bool itas109::CSerialPort::open()
{
	return p_serialPortBase->open();
}

void itas109::CSerialPort::close()
{
	p_serialPortBase->close();
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
	return "CSerialPort V4.0.0.181208";
}

void itas109::CSerialPort::onReadReady()
{
	readReady.emit();
	/*	char * receiveData = new char[256];
		int size = readData(receiveData, 256);
		if (size == 0 || size == -1)
		{
		std::cout << "received NULL" << std::endl;
		}
		else
		{
		receiveData[size + 1] = '\0';
		std::cout << "received : " << receiveData << std::endl;
		readReady.emit();
		}*/
}
