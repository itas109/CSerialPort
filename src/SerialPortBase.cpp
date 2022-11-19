#include "CSerialPort/SerialPortBase.h"
#include "CSerialPort/ithread.hpp"
#include "CSerialPort/itimer.hpp"

CSerialPortBase::CSerialPortBase()
    : m_lastError(0)
    , m_operateMode(itas109::AsynchronousOperate)
    , m_readIntervalTimeoutMS(50)
    , m_minByteReadNotify(1)
    , p_mutex(NULL)
    , p_readEvent(NULL)
    , p_timer(NULL)
{
    p_mutex = new itas109::IMutex();
    p_timer = new itas109::ITimer<itas109::CSerialPortListener>();
}

CSerialPortBase::CSerialPortBase(const char *portName)
    : m_lastError(0)
    , m_operateMode(itas109::AsynchronousOperate)
    , m_readIntervalTimeoutMS(50)
    , m_minByteReadNotify(1)
    , p_mutex(NULL)
    , p_readEvent(NULL)
    , p_timer(NULL)

{
    p_mutex = new itas109::IMutex();
    p_timer = new itas109::ITimer<itas109::CSerialPortListener>();
}

CSerialPortBase::~CSerialPortBase()
{
    if (p_mutex)
    {
        delete p_mutex;
        p_mutex = NULL;
    }

    if (p_timer)
    {
        delete p_timer;
        p_timer = NULL;
    }
}

void CSerialPortBase::setOperateMode(itas109::OperateMode operateMode)
{
    m_operateMode = operateMode;
}

unsigned int CSerialPortBase::getReadIntervalTimeout()
{
    return m_readIntervalTimeoutMS;
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

int CSerialPortBase::connectReadEvent(itas109::CSerialPortListener *event)
{
    if (event)
    {
        p_readEvent = event;
        return itas109::NoError;
    }
    else
    {
        return itas109::InvalidParameterError;
    }
}

int CSerialPortBase::disconnectReadEvent()
{
    p_readEvent = NULL;
    return itas109::NoError;
}