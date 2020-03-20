#include "SerialPort.h"

#ifdef I_OS_WIN
#include "SerialPortWinBase.h"
#define CSERIALPORTBASE CSerialPortWinBase
#elif defined I_OS_UNIX
#include "SerialPortUnixBase.h"
#define CSERIALPORTBASE CSerialPortUnixBase
#else
//Not support
#define CSERIALPORTBASE
#endif // I_OS_WIN

#include <iostream>

using namespace itas109;

CSerialPort::CSerialPort()
{
    p_serialPortBase = new CSERIALPORTBASE();

    p_serialPortBase->setMinByteReadNoify(1);

    ((CSERIALPORTBASE *)p_serialPortBase)->readReady.connect(this, &CSerialPort::onReadReady);
}

itas109::CSerialPort::CSerialPort(const std::string & portName)
{
    p_serialPortBase = new CSERIALPORTBASE(portName);

    p_serialPortBase->setMinByteReadNoify(1);

    ((CSERIALPORTBASE *)p_serialPortBase)->readReady.connect(this, &CSerialPort::onReadReady);
}


CSerialPort::~CSerialPort()
{
    ((CSERIALPORTBASE *)p_serialPortBase)->readReady.disconnect_all();

    if(p_serialPortBase)
    {
        delete p_serialPortBase;
        p_serialPortBase = NULL;
    }
}

void itas109::CSerialPort::init(std::string portName, int baudRate /*= itas109::BaudRate::BaudRate9600*/, itas109::Parity parity /*= itas109::Parity::ParityNone*/, itas109::DataBits dataBits /*= itas109::DataBits::DataBits8*/, itas109::StopBits stopbits /*= itas109::StopBits::StopOne*/, itas109::FlowConctrol flowConctrol /*= itas109::FlowConctrol::FlowNone*/, int64 readBufferSize /*= 512*/)
{
    if(p_serialPortBase)
    {
        p_serialPortBase->init(portName, baudRate, parity, dataBits, stopbits, flowConctrol, readBufferSize);
    }
}

void itas109::CSerialPort::init(int port, int baudRate /*= itas109::BaudRate9600*/, itas109::Parity parity /*= itas109::ParityNone*/, itas109::DataBits dataBits /*= itas109::DataBits8*/, itas109::StopBits stopbits /*= itas109::StopOne*/, itas109::FlowConctrol flowConctrol /*= itas109::FlowNone*/, int64 readBufferSize /*= 512*/)
{
    if(p_serialPortBase)
    {
        p_serialPortBase->init(port, baudRate, parity, dataBits, stopbits, flowConctrol, readBufferSize);
    }
}

void CSerialPort::setOperateMode(OperateMode operateMode)
{
    if(p_serialPortBase)
    {
        p_serialPortBase->setOperateMode(operateMode);}
}

bool itas109::CSerialPort::open()
{
    if(p_serialPortBase)
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
    if(p_serialPortBase)
    {
        p_serialPortBase->closePort();
    }
}

bool itas109::CSerialPort::isOpened()
{
    if(p_serialPortBase)
    {
        return p_serialPortBase->isOpened();
    }
    else
    {
        return false;
    }
}

int itas109::CSerialPort::readData(char *data, int maxSize)
{
    if(p_serialPortBase)
    {
        return p_serialPortBase->readData(data,maxSize);
    }
    else
    {
        return -1;
    }
}


int itas109::CSerialPort::readAllData(char *data)
{
    if(p_serialPortBase)
    {
        return p_serialPortBase->readAllData(data);
    }
    else
    {
        return -1;
    }
}

int itas109::CSerialPort::readLineData(char *data, int maxSize)
{
    if(p_serialPortBase)
    {
        return p_serialPortBase->readLineData(data,maxSize);
    }
    else
    {
        return -1;
    }
}

int itas109::CSerialPort::writeData(const char * data, int maxSize)
{
    if(p_serialPortBase)
    {
        return p_serialPortBase->writeData(data,maxSize);
    }
    else
    {
        return -1;
    }
}

void itas109::CSerialPort::setDebugModel(bool isDebug)
{
    if(p_serialPortBase)
    {
        p_serialPortBase->setDebugModel(isDebug);
    }
}

void itas109::CSerialPort::setReadTimeInterval(int msecs)
{
    if(p_serialPortBase)
    {
        p_serialPortBase->setReadTimeInterval(msecs);
    }
}

void CSerialPort::setMinByteReadNoify(unsigned int minByteReadNoify)
{
    if(p_serialPortBase)
    {
        p_serialPortBase->setMinByteReadNoify(minByteReadNoify);
    }
}

int itas109::CSerialPort::getLastError() const
{
    if(p_serialPortBase)
    {
        return p_serialPortBase->getLastError();
    }
    else
    {
        // null error
        return itas109::/*SerialPortError::*/SystemError;
    }
}

void itas109::CSerialPort::clearError()
{
    if(p_serialPortBase)
    {
        p_serialPortBase->clearError();
    }
}

void itas109::CSerialPort::setPortName(std::string portName)
{
    if(p_serialPortBase)
    {
        p_serialPortBase->setPortName(portName);
    }
}

std::string itas109::CSerialPort::getPortName() const
{
    if(p_serialPortBase)
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
    if(p_serialPortBase)
    {
        p_serialPortBase->setBaudRate(baudRate);
    }
}

int itas109::CSerialPort::getBaudRate() const
{
    if(p_serialPortBase)
    {
        return p_serialPortBase->getBaudRate();
    }
    else
    {
        return -1;
    }
}

void itas109::CSerialPort::setParity(itas109::Parity parity)
{
    if(p_serialPortBase)
    {
        p_serialPortBase->setParity(parity);
    }
}

itas109::Parity itas109::CSerialPort::getParity() const
{
    if(p_serialPortBase)
    {
        return  p_serialPortBase->getParity();
    }
    else
    {
        // should retrun error
        return itas109::/*Parity::*/ParityNone;
    }
}

void itas109::CSerialPort::setDataBits(itas109::DataBits dataBits)
{
    if(p_serialPortBase)
    {
        p_serialPortBase->setDataBits(dataBits);
    }
}

itas109::DataBits itas109::CSerialPort::getDataBits() const
{
    if(p_serialPortBase)
    {
        return p_serialPortBase->getDataBits();
    }
    else
    {
        // should retrun error
        return itas109::/*DataBits::*/DataBits8;
    }
}

void itas109::CSerialPort::setStopBits(itas109::StopBits stopbits)
{
    if(p_serialPortBase)
    {
        p_serialPortBase->setStopBits(stopbits);
    }
}

itas109::StopBits itas109::CSerialPort::getStopBits() const
{
    if(p_serialPortBase)
    {
        return p_serialPortBase->getStopBits();
    }
    else
    {
        // should retrun error
        return itas109::/*StopBits::*/StopOne;
    }
}

void itas109::CSerialPort::setFlowConctrol(itas109::FlowConctrol flowConctrol)
{
    if(p_serialPortBase)
    {
        p_serialPortBase->setFlowConctrol(flowConctrol);
    }
}

itas109::FlowConctrol itas109::CSerialPort::getFlowConctrol() const
{
    if(p_serialPortBase)
    {
        return p_serialPortBase->getFlowConctrol();
    }
    else
    {
        // should retrun error
        return itas109::/*FlowConctrol::*/FlowNone;
    }
}

void itas109::CSerialPort::setReadBufferSize(int64 size)
{
    if(p_serialPortBase)
    {
        p_serialPortBase->setReadBufferSize(size);
    }
}

int64 itas109::CSerialPort::getReadBufferSize() const
{
    if(p_serialPortBase)
    {
        return p_serialPortBase->getReadBufferSize();
    }
    else
    {
        return -1;
    }
}

void itas109::CSerialPort::setDtr(bool set /*= true*/)
{
    if(p_serialPortBase)
    {
        p_serialPortBase->setDtr(set);
    }
}

void itas109::CSerialPort::setRts(bool set /*= true*/)
{
    if(p_serialPortBase)
    {
        p_serialPortBase->setRts(set);
    }
}

std::string itas109::CSerialPort::getVersion()
{
    return "https://github.com/itas109/CSerialPort (itas109@qq.com) - CSerialPort V4.0.2.200108";
}

void itas109::CSerialPort::onReadReady()
{
    readReady._emit();
}
