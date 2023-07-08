#include <iostream>

#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPort_version.h"
#include "CSerialPort/iutils.hpp"
#include "CSerialPort/itimer.hpp"

#ifdef I_OS_WIN
#include "CSerialPort/SerialPortWinBase.h"
#define CSERIALPORTBASE CSerialPortWinBase
#elif defined I_OS_UNIX
#include "CSerialPort/SerialPortUnixBase.h"
#define CSERIALPORTBASE CSerialPortUnixBase
#else
// Not support
#define CSERIALPORTBASE
#endif // I_OS_WIN

using namespace itas109;

CSerialPort::CSerialPort()
    : p_serialPortBase(NULL)
{
    p_serialPortBase = new CSERIALPORTBASE();

    p_serialPortBase->setReadIntervalTimeout(0);
    p_serialPortBase->setMinByteReadNotify(1);
}

itas109::CSerialPort::CSerialPort(const char *portName)
    : p_serialPortBase(NULL)
{
    p_serialPortBase = new CSERIALPORTBASE(portName);

    p_serialPortBase->setReadIntervalTimeout(0);
    p_serialPortBase->setMinByteReadNotify(1);
}

CSerialPort::~CSerialPort()
{
    if (p_serialPortBase)
    {
        delete p_serialPortBase;
        p_serialPortBase = NULL;
    }
}

void itas109::CSerialPort::init(const char *portName,
                                int baudRate /*= itas109::BaudRate::BaudRate9600*/,
                                itas109::Parity parity /*= itas109::Parity::ParityNone*/,
                                itas109::DataBits dataBits /*= itas109::DataBits::DataBits8*/,
                                itas109::StopBits stopbits /*= itas109::StopBits::StopOne*/,
                                itas109::FlowControl flowControl /*= itas109::FlowControl::FlowNone*/,
                                unsigned int readBufferSize /*= 4096*/)
{
    if (p_serialPortBase)
    {
        p_serialPortBase->init(portName, baudRate, parity, dataBits, stopbits, flowControl, readBufferSize);
    }
}

void CSerialPort::setOperateMode(OperateMode operateMode)
{
    if (p_serialPortBase)
    {
        p_serialPortBase->setOperateMode(operateMode);
    }
}

bool itas109::CSerialPort::open()
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->openPort();
    }
    else
    {
        return false;
    }
}

void itas109::CSerialPort::close()
{
    if (p_serialPortBase)
    {
        p_serialPortBase->closePort();
    }
}

bool itas109::CSerialPort::isOpen()
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->isOpen();
    }
    else
    {
        return false;
    }
}

int itas109::CSerialPort::connectReadEvent(itas109::CSerialPortListener *event)
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->connectReadEvent(event);
    }
    else
    {
        return itas109::ErrorNullPointer;
    }
}

int itas109::CSerialPort::disconnectReadEvent()
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->disconnectReadEvent();
    }
    else
    {
        return itas109::ErrorNullPointer;
    }
}

unsigned int itas109::CSerialPort::getReadBufferUsedLen() const
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->getReadBufferUsedLen();
    }
    else
    {
        return itas109::ErrorNullPointer;
    }
}

int itas109::CSerialPort::readData(void *data, int size)
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->readData(data, size);
    }
    else
    {
        return itas109::ErrorNullPointer;
    }
}

int itas109::CSerialPort::readAllData(void *data)
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->readAllData(data);
    }
    else
    {
        return itas109::ErrorNullPointer;
    }
}

int itas109::CSerialPort::readLineData(void *data, int size)
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->readLineData(data, size);
    }
    else
    {
        return itas109::ErrorNullPointer;
    }
}

int itas109::CSerialPort::writeData(const void *data, int size)
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->writeData(data, size);
    }
    else
    {
        return itas109::ErrorNullPointer;
    }
}

void itas109::CSerialPort::setDebugModel(bool isDebug)
{
    if (p_serialPortBase)
    {
        p_serialPortBase->setDebugModel(isDebug);
    }
}

void itas109::CSerialPort::setReadIntervalTimeout(unsigned int msecs)
{
    if (p_serialPortBase)
    {
        p_serialPortBase->setReadIntervalTimeout(msecs);
    }
}

bool itas109::CSerialPort::flushBuffers()
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->flushBuffers();
    }
    else
    {
        return false;
    }
}

bool itas109::CSerialPort::flushReadBuffers()
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->flushReadBuffers();
    }
    else
    {
        return false;
    }
}

bool itas109::CSerialPort::flushWriteBuffers()
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->flushWriteBuffers();
    }
    else
    {
        return false;
    }
}
unsigned int itas109::CSerialPort::getReadIntervalTimeout()
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->getReadIntervalTimeout();
    }
    else
    {
        return 0;
    }
}

void CSerialPort::setMinByteReadNotify(unsigned int minByteReadNotify)
{
    if (p_serialPortBase)
    {
        p_serialPortBase->setMinByteReadNotify(minByteReadNotify);
    }
}

int itas109::CSerialPort::getLastError() const
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->getLastError();
    }
    else
    {
        // null error
        return itas109::/*SerialPortError::*/ ErrorNullPointer;
    }
}

const char *itas109::CSerialPort::getLastErrorMsg() const
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->getLastErrorMsg();
    }
    else
    {
        return "";
    }
}

void itas109::CSerialPort::clearError()
{
    if (p_serialPortBase)
    {
        p_serialPortBase->clearError();
    }
}

void itas109::CSerialPort::setPortName(const char *portName)
{
    if (p_serialPortBase)
    {
        p_serialPortBase->setPortName(portName);
    }
}

const char *itas109::CSerialPort::getPortName() const
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->getPortName();
    }
    else
    {
        return "";
    }
}

void itas109::CSerialPort::setBaudRate(int baudRate)
{
    if (p_serialPortBase)
    {
        p_serialPortBase->setBaudRate(baudRate);
    }
}

int itas109::CSerialPort::getBaudRate() const
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->getBaudRate();
    }
    else
    {
        return itas109::ErrorNullPointer;
    }
}

void itas109::CSerialPort::setParity(itas109::Parity parity)
{
    if (p_serialPortBase)
    {
        p_serialPortBase->setParity(parity);
    }
}

itas109::Parity itas109::CSerialPort::getParity() const
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->getParity();
    }
    else
    {
        // should retrun error
        return itas109::/*Parity::*/ ParityNone;
    }
}

void itas109::CSerialPort::setDataBits(itas109::DataBits dataBits)
{
    if (p_serialPortBase)
    {
        p_serialPortBase->setDataBits(dataBits);
    }
}

itas109::DataBits itas109::CSerialPort::getDataBits() const
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->getDataBits();
    }
    else
    {
        // should retrun error
        return itas109::/*DataBits::*/ DataBits8;
    }
}

void itas109::CSerialPort::setStopBits(itas109::StopBits stopbits)
{
    if (p_serialPortBase)
    {
        p_serialPortBase->setStopBits(stopbits);
    }
}

itas109::StopBits itas109::CSerialPort::getStopBits() const
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->getStopBits();
    }
    else
    {
        // should retrun error
        return itas109::/*StopBits::*/ StopOne;
    }
}

void itas109::CSerialPort::setFlowControl(itas109::FlowControl flowControl)
{
    if (p_serialPortBase)
    {
        p_serialPortBase->setFlowControl(flowControl);
    }
}

itas109::FlowControl itas109::CSerialPort::getFlowControl() const
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->getFlowControl();
    }
    else
    {
        // should retrun error
        return itas109::/*FlowControl::*/ FlowNone;
    }
}

void itas109::CSerialPort::setReadBufferSize(unsigned int size)
{
    if (p_serialPortBase)
    {
        p_serialPortBase->setReadBufferSize(size);
    }
}

unsigned int itas109::CSerialPort::getReadBufferSize() const
{
    if (p_serialPortBase)
    {
        return p_serialPortBase->getReadBufferSize();
    }
    else
    {
        return itas109::ErrorNullPointer;
    }
}

void itas109::CSerialPort::setDtr(bool set /*= true*/)
{
    if (p_serialPortBase)
    {
        p_serialPortBase->setDtr(set);
    }
}

void itas109::CSerialPort::setRts(bool set /*= true*/)
{
    if (p_serialPortBase)
    {
        p_serialPortBase->setRts(set);
    }
}

const char *itas109::CSerialPort::getVersion()
{
    static char version[256];
    itas109::IUtils::strncpy(version, "https://github.com/itas109/CSerialPort - V", 256);
    return itas109::IUtils::strncat(version, CSERIALPORT_VERSION, 20);
}
