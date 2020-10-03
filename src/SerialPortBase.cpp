#include "CSerialPort/SerialPortBase.h"

CSerialPortBase::CSerialPortBase()
{
    lastError = 0;

    m_minByteReadNotify = 1;

    m_operateMode = itas109::AsynchronousOperate;
}

CSerialPortBase::CSerialPortBase(const std::string &portName)
{
    lastError = 0;

    m_minByteReadNotify = 1;

    m_operateMode = itas109::AsynchronousOperate;
}

CSerialPortBase::~CSerialPortBase() {}

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

void CSerialPortBase::clearError() {}
