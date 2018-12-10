#include "SerialPortUnixBase.h"

sigslot::signal0<> CSerialPortUnixBase::readReady;//sigslot

CSerialPortUnixBase::CSerialPortUnixBase()
{
}

CSerialPortUnixBase::CSerialPortUnixBase(const std::string & portName)
{

}


CSerialPortUnixBase::~CSerialPortUnixBase()
{
}

void CSerialPortUnixBase::construct()
{

}

void CSerialPortUnixBase::init(std::string portName, int baudRate /*= itas109::BaudRate::BaudRate9600*/, itas109::Parity parity /*= itas109::Parity::ParityNone*/, itas109::DataBits dataBits /*= itas109::DataBits::DataBits8*/, itas109::StopBits stopbits /*= itas109::StopBits::StopOne*/, itas109::FlowConctrol flowConctrol /*= itas109::FlowConctrol::FlowNone*/, int64 readBufferSize /*= 512*/)
{

}

bool CSerialPortUnixBase::open()
{
	bool bRet = false;

	lock();

	unlock();

	return bRet;
}

void CSerialPortUnixBase::close()
{

}

bool CSerialPortUnixBase::isOpened()
{
	bool bRet = false;

	return bRet;
}

int CSerialPortUnixBase::readData(char *data, int maxSize)
{
	int iRet = -1;
	lock();

	unlock();

	return iRet;
}

int CSerialPortUnixBase::readLineData(char *data, int maxSize)
{
	int iRet = -1;
	lock();

	unlock();

	return iRet;
}

int CSerialPortUnixBase::writeData(const char * data, int maxSize)
{
	int iRet = -1;
	lock();

	unlock();

	return iRet;
}

void CSerialPortUnixBase::setDebugModel(bool isDebug)
{

}

void CSerialPortUnixBase::setReadTimeInterval(int msecs)
{

}

int CSerialPortUnixBase::getLastError() const
{
	return lastError;
}

void CSerialPortUnixBase::clearError()
{
	lastError = itas109::NoError;
}

void CSerialPortUnixBase::setPortName(std::string portName)
{
	m_portName = portName;
}

std::string CSerialPortUnixBase::getPortName() const
{
	return m_portName;
}

void CSerialPortUnixBase::setBaudRate(int baudRate)
{
	m_baudRate = baudRate;
}

int CSerialPortUnixBase::getBaudRate() const
{
	return m_baudRate;
}

void CSerialPortUnixBase::setParity(itas109::Parity parity)
{
	m_parity = parity;
}

itas109::Parity CSerialPortUnixBase::getParity() const
{
	return m_parity;
}

void CSerialPortUnixBase::setDataBits(itas109::DataBits dataBits)
{
	m_dataBits = dataBits;
}

itas109::DataBits CSerialPortUnixBase::getDataBits() const
{
	return m_dataBits;
}

void CSerialPortUnixBase::setStopBits(itas109::StopBits stopbits)
{
	m_stopbits = stopbits;
}

itas109::StopBits CSerialPortUnixBase::getStopBits() const
{
	return m_stopbits;
}

void CSerialPortUnixBase::setFlowConctrol(itas109::FlowConctrol flowConctrol)
{
	m_flowConctrol = flowConctrol;
}

itas109::FlowConctrol CSerialPortUnixBase::getFlowConctrol() const
{
	return m_flowConctrol;
}

void CSerialPortUnixBase::setReadBufferSize(int64 size)
{
	m_readBufferSize = size;
}

int64 CSerialPortUnixBase::getReadBufferSize() const
{
	return m_readBufferSize;
}

void CSerialPortUnixBase::setDtr(bool set /*= true*/)
{

}

void CSerialPortUnixBase::setRts(bool set /*= true*/)
{

}

std::string CSerialPortUnixBase::getVersion()
{
	std::string m_version = "CSerialPortUnixBase V1.0.0.181119";
	return m_version;
}

void CSerialPortUnixBase::lock()
{

}

void CSerialPortUnixBase::unlock()
{

}
