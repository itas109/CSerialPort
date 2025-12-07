#include "CSerialPort/SerialPortAsyncBase.h"

#include "CSerialPort/ithread.hpp"
#include "CSerialPort/itimer.hpp"
#include "CSerialPort/IProtocolParser.h"
#include "CSerialPort/SerialPortHotPlug.hpp"

CSerialPortAsyncBase::CSerialPortAsyncBase()
    : CSerialPortAsyncBase("")
{
}

CSerialPortAsyncBase::CSerialPortAsyncBase(const char *portName)
    : CSerialPortBase(portName)
    , m_readIntervalTimeoutMS(50)
    , m_minByteReadNotify(1)
    , m_byteReadBufferFullNotify(3276) // 4096*0.8
    , p_readEvent(nullptr)
    , p_timer(nullptr)
    , p_serialPortHotPlug(nullptr)
    , p_protocolParser(nullptr)
{
    m_operateMode = itas109::AsynchronousOperate;
    p_timer = new itas109::ITimer<itas109::CSerialPortListener>();
}

CSerialPortAsyncBase::~CSerialPortAsyncBase()
{
    if (p_timer)
    {
        delete p_timer;
        p_timer = nullptr;
    }
}

unsigned int CSerialPortAsyncBase::getReadIntervalTimeout()
{
    return m_readIntervalTimeoutMS;
}

unsigned int CSerialPortAsyncBase::getMinByteReadNotify()
{
    return m_minByteReadNotify;
}

void CSerialPortAsyncBase::setByteReadBufferFullNotify(unsigned int byteReadBufferFullNotify)
{
    m_byteReadBufferFullNotify = byteReadBufferFullNotify;
}

unsigned int CSerialPortAsyncBase::getByteReadBufferFullNotify()
{
    return m_byteReadBufferFullNotify;
}

int CSerialPortAsyncBase::connectReadEvent(itas109::CSerialPortListener *event)
{
    if (event)
    {
        p_readEvent = event;
        return itas109::ErrorOK;
    }
    else
    {
        return itas109::ErrorInvalidParam;
    }
}

int CSerialPortAsyncBase::disconnectReadEvent()
{
    p_readEvent = nullptr;
    return itas109::ErrorOK;
}

int CSerialPortAsyncBase::connectHotPlugEvent(itas109::CSerialPortHotPlugListener *event)
{
    if (event)
    {
        if (nullptr == p_serialPortHotPlug)
        {
            p_serialPortHotPlug = new itas109::CSerialPortHotPlug();
        }
        p_serialPortHotPlug->connectHotPlugEvent(event);

        return itas109::ErrorOK;
    }
    else
    {
        return itas109::ErrorInvalidParam;
    }
}

int CSerialPortAsyncBase::disconnectHotPlugReadEvent()
{
    if (p_serialPortHotPlug)
    {
        p_serialPortHotPlug->disconnectHotPlugEvent();

        delete p_serialPortHotPlug;
        p_serialPortHotPlug = nullptr;

        return itas109::ErrorOK;
    }
    else
    {
        return itas109::ErrorInvalidParam;
    }
}

int CSerialPortAsyncBase::setProtocolParser(itas109::IProtocolParser *parser)
{
    if (parser)
    {
        p_protocolParser = parser;
        return itas109::ErrorOK;
    }
    else
    {
        return itas109::ErrorInvalidParam;
    }
}
