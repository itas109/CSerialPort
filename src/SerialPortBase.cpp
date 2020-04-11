#include "SerialPortBase.h"


CSerialPortBase::CSerialPortBase()
{
	m_minByteReadNoify = 1;
}

CSerialPortBase::CSerialPortBase(const std::string & portName)
{

}

CSerialPortBase::~CSerialPortBase()
{
}

void CSerialPortBase::setOperateMode(itas109::OperateMode operateMode)
{
    m_operateMode = operateMode;
}

unsigned int CSerialPortBase::getMinByteReadNoify()
{
    return m_minByteReadNoify;
}

int CSerialPortBase::getLastError() const
{
	return lastError;
}

void CSerialPortBase::clearError()
{

}


