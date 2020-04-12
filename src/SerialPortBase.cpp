#include "SerialPortBase.h"


CSerialPortBase::CSerialPortBase()
{
    m_minByteReadNotify = 1;
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

unsigned int CSerialPortBase::getMinByteReadNotify()
{
    return m_minByteReadNotify;
}

int CSerialPortBase::getLastError() const
{
	return lastError;
}

void CSerialPortBase::clearError()
{

}


