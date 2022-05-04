#include "CSerialPort/SerialPortBase.h"
#include "CSerialPort/ithread.hpp"

CSerialPortBase::CSerialPortBase()
    : p_mutex(NULL)
    , m_lastError(0)
    , m_minByteReadNotify(1)
    , m_operateMode(itas109::AsynchronousOperate)
{
    p_mutex = new itas109::IMutex();
}

CSerialPortBase::CSerialPortBase(const std::string &portName)
    : p_mutex(NULL)
    , m_lastError(0)
    , m_minByteReadNotify(1)
    , m_operateMode(itas109::AsynchronousOperate)
{
    p_mutex = new itas109::IMutex();
}

CSerialPortBase::~CSerialPortBase()
{
    if (p_mutex)
    {
        delete p_mutex;
        p_mutex = NULL;
    }
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
    return m_lastError;
}

void CSerialPortBase::clearError() {}
