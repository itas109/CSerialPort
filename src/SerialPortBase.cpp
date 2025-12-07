#include "CSerialPort/SerialPortBase.h"

#include "CSerialPort/iutils.hpp"

CSerialPortBase::CSerialPortBase()
    : CSerialPortBase{""}
{
}

CSerialPortBase::CSerialPortBase(const char *portName)
    : m_lastError{itas109::ErrorOK}
    , m_operateMode{itas109::NativeSynchronousOperate}
{
    itas109::IUtils::strncpy(m_portName, portName, 256);
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
    return m_lastError;
}

const char *CSerialPortBase::getLastErrorMsg() const
{
    switch (m_lastError)
    {
        case itas109::ErrorOK:
            return "success";
        case itas109::ErrorUnknown:
            return "unknown error";
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