#include "CSerialPort/SerialPortBase.h"

#include "CSerialPort/iutils.hpp"

CSerialPortBase::CSerialPortBase()
    : CSerialPortBase{""}
{
}

CSerialPortBase::CSerialPortBase(const char *portName)
    : m_lastError{itas109::ErrorOK}
    , m_operateMode{itas109::NativeSynchronousOperate}
    , m_baudRate(itas109::BaudRate9600)
    , m_parity(itas109::ParityNone)
    , m_dataBits(itas109::DataBits8)
    , m_stopbits(itas109::StopOne)
    , m_flowControl(itas109::FlowNone)
    , m_readBufferSize(4096)
{
    itas109::IUtils::strncpy(m_portName, portName, 256);
}

CSerialPortBase::~CSerialPortBase()
{
}

void CSerialPortBase::init(const char *portName,
                           int baudRate /*= itas109::BaudRate::BaudRate9600*/,
                           itas109::Parity parity /*= itas109::Parity::ParityNone*/,
                           itas109::DataBits dataBits /*= itas109::DataBits::DataBits8*/,
                           itas109::StopBits stopbits /*= itas109::StopBits::StopOne*/,
                           itas109::FlowControl flowControl /*= itas109::FlowControl::FlowNone*/,
                           unsigned int readBufferSize /*= 4096*/)
{
    itas109::IUtils::strncpy(m_portName, portName, 256);
    m_baudRate = baudRate;
    m_parity = parity;
    m_dataBits = dataBits;
    m_stopbits = stopbits;
    m_flowControl = flowControl;
    m_readBufferSize = readBufferSize;
}

void CSerialPortBase::setOperateMode(itas109::OperateMode operateMode)
{
    m_operateMode = operateMode;
}

int CSerialPortBase::readAllData(void *data)
{
    return readData(data, getReadBufferUsedLen());
}

void CSerialPortBase::setDebugModel(bool isDebug)
{
    // TODO
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

void CSerialPortBase::setPortName(const char *portName)
{
    // windows: COM1 unix: /dev/ttyS0
    itas109::IUtils::strncpy(m_portName, portName, 256);
}

const char *CSerialPortBase::getPortName() const
{
    return m_portName;
}

void CSerialPortBase::setBaudRate(int baudRate)
{
    m_baudRate = baudRate;
}

int CSerialPortBase::getBaudRate() const
{
    return m_baudRate;
}

void CSerialPortBase::setParity(itas109::Parity parity)
{
    m_parity = parity;
}

itas109::Parity CSerialPortBase::getParity() const
{
    return m_parity;
}

void CSerialPortBase::setDataBits(itas109::DataBits dataBits)
{
    m_dataBits = dataBits;
}

itas109::DataBits CSerialPortBase::getDataBits() const
{
    return m_dataBits;
}

void CSerialPortBase::setStopBits(itas109::StopBits stopbits)
{
    m_stopbits = stopbits;
}

itas109::StopBits CSerialPortBase::getStopBits() const
{
    return m_stopbits;
}

void CSerialPortBase::setFlowControl(itas109::FlowControl flowControl)
{
    m_flowControl = flowControl;
}

itas109::FlowControl CSerialPortBase::getFlowControl() const
{
    return m_flowControl;
}

void CSerialPortBase::setReadBufferSize(unsigned int size)
{
    m_readBufferSize = size;
}

unsigned int CSerialPortBase::getReadBufferSize() const
{
    return m_readBufferSize;
}