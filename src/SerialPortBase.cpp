#include "CSerialPort/SerialPortBase.h"
#include "CSerialPort/ithread.hpp"
#include "CSerialPort/itimer.hpp"
#include "CSerialPort/IProtocolParser.h"
#include "CSerialPort/SerialPortHotPlug.hpp"

CSerialPortBase::CSerialPortBase()
    : CSerialPortBase("")
{
}

CSerialPortBase::CSerialPortBase(const char *portName)
    : m_lastError(0)
    , m_operateMode(itas109::AsynchronousOperate)
    , m_readIntervalTimeoutMS(50)
    , m_minByteReadNotify(1)
    , m_byteReadBufferFullNotify(3276) // 4096*0.8
    , p_readEvent(nullptr)
    , p_timer(nullptr)
    , p_serialPortHotPlug(nullptr)
    , p_protocolParser(nullptr)
{
    p_timer = new itas109::ITimer<itas109::CSerialPortListener>();
}

CSerialPortBase::~CSerialPortBase()
{
    if (p_timer)
    {
        delete p_timer;
        p_timer = nullptr;
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

void CSerialPortBase::setByteReadBufferFullNotify(unsigned int byteReadBufferFullNotify)
{
    m_byteReadBufferFullNotify = byteReadBufferFullNotify;
}

unsigned int CSerialPortBase::getByteReadBufferFullNotify()
{
    return m_byteReadBufferFullNotify;
}

int CSerialPortBase::getLastError() const
{
    return m_lastError;
}

const char *CSerialPortBase::getLastErrorMsg() const
{
    switch (m_lastError)
    {
        case itas109::ErrorUnknown:
            return "unknown error";
        case itas109::ErrorOK:
            return "success";
        case itas109::ErrorFail:
            return "general error";
        case itas109::ErrorNotImplemented:
            return "not implemented error";
        case itas109::ErrorInner:
            return "innet error";
        case itas109::ErrorNullPointer:
            return "null pointer error";
        case itas109::ErrorInvalidParam:
            return "invalid param error";
        case itas109::ErrorAccessDenied:
            return "access denied error";
        case itas109::ErrorOutOfMemory:
            return "out of memory error";
        case itas109::ErrorTimeout:
            return "timeout error";
        case itas109::ErrorNotInit:
            return "not init error";
        case itas109::ErrorInitFailed:
            return "init failed error";
        case itas109::ErrorAlreadyExist:
            return "already exist error";
        case itas109::ErrorNotExist:
            return "not exist error";
        case itas109::ErrorAlreadyOpen:
            return "already open error";
        case itas109::ErrorNotOpen:
            return "not open error";
        case itas109::ErrorOpenFailed:
            return "open failed error";
        case itas109::ErrorCloseFailed:
            return "close failed error";
        case itas109::ErrorWriteFailed:
            return "write failed error";
        case itas109::ErrorReadFailed:
            return "read failed error";
        default:
            return "undefined error code";
            break;
    }
}

void CSerialPortBase::clearError()
{
    m_lastError = itas109::ErrorOK;
}

int CSerialPortBase::connectReadEvent(itas109::CSerialPortListener *event)
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

int CSerialPortBase::disconnectReadEvent()
{
    p_readEvent = nullptr;
    return itas109::ErrorOK;
}

int CSerialPortBase::connectHotPlugEvent(itas109::CSerialPortHotPlugListener *event)
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

int CSerialPortBase::disconnectHotPlugReadEvent()
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

int CSerialPortBase::setProtocolParser(itas109::IProtocolParser *parser)
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
