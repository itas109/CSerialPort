#include "SerialPortBase.h"


CSerialPortBase::CSerialPortBase()
{
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

int CSerialPortBase::getLastError() const
{
	return lastError;
}

void CSerialPortBase::clearError()
{

}


