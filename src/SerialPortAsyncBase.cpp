#include "CSerialPort/SerialPortAsyncBase.h"

#include "CSerialPort/ithread.hpp"
#include "CSerialPort/ibuffer.hpp"
#include "CSerialPort/itimer.hpp"
#include "CSerialPort/IProtocolParser.h"
#include "CSerialPort/SerialPortHotPlug.hpp"

CSerialPortAsyncBase::CSerialPortAsyncBase()
    : CSerialPortAsyncBase("")
{
}

CSerialPortAsyncBase::CSerialPortAsyncBase(const char *portName)
    : CSerialPortBase(portName)
    , m_readIntervalTimeoutMS(0)
    , m_minByteReadNotify(2)
    , m_byteReadBufferFullNotify(3276) // 4096*0.8
    , p_readBuffer(nullptr)
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
    if (p_readBuffer)
    {
        delete p_readBuffer;
        p_readBuffer = nullptr;
    }

    if (p_timer)
    {
        delete p_timer;
        p_timer = nullptr;
    }
}

void CSerialPortAsyncBase::init(const char *portName,
                                int baudRate /*= itas109::BaudRate::BaudRate9600*/,
                                itas109::Parity parity /*= itas109::Parity::ParityNone*/,
                                itas109::DataBits dataBits /*= itas109::DataBits::DataBits8*/,
                                itas109::StopBits stopbits /*= itas109::StopBits::StopOne*/,
                                itas109::FlowControl flowControl /*= itas109::FlowControl::FlowNone*/,
                                unsigned int readBufferSize /*= 4096*/)
{
    CSerialPortBase::init(portName, baudRate, parity, dataBits, stopbits, flowControl, readBufferSize);

    m_byteReadBufferFullNotify = (unsigned int)(m_readBufferSize * 0.8);

    if (p_readBuffer)
    {
        delete p_readBuffer;
        p_readBuffer = nullptr;
    }
    p_readBuffer = new itas109::RingBuffer<char>(m_readBufferSize);
}

void CSerialPortAsyncBase::setReadIntervalTimeout(unsigned int msecs)
{
    m_readIntervalTimeoutMS = msecs;
}

unsigned int CSerialPortAsyncBase::getReadIntervalTimeout() const
{
    return m_readIntervalTimeoutMS;
}

void CSerialPortAsyncBase::setMinByteReadNotify(unsigned int minByteReadNotify)
{
    m_minByteReadNotify = minByteReadNotify;
}

unsigned int CSerialPortAsyncBase::getMinByteReadNotify() const
{
    return m_minByteReadNotify;
}

void CSerialPortAsyncBase::setByteReadBufferFullNotify(unsigned int byteReadBufferFullNotify)
{
    m_byteReadBufferFullNotify = byteReadBufferFullNotify;
}

unsigned int CSerialPortAsyncBase::getByteReadBufferFullNotify() const
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
