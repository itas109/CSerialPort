#include "cserialport.h"

#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
#include "CSerialPort/iutils.hpp"

#include <vector>

class CSPListener : public itas109::CSerialPortListener
{
public:
    CSPListener(void *pSerialPort, pFunReadEvent pFun)
        : m_pSerialPort(pSerialPort)
        , m_pFun(pFun){};

    void onReadEvent(const char *portName, unsigned int readBufferLen)
    {
        m_pFun(m_pSerialPort, portName, readBufferLen);
    };

private:
    void *m_pSerialPort;
    pFunReadEvent m_pFun;
};

void CSerialPortAvailablePortInfos(SerialPortInfoArray *portInfoArray)
{
    std::vector<itas109::SerialPortInfo> portInfos = itas109::CSerialPortInfo::availablePortInfos();
    portInfoArray->size = static_cast<unsigned int>(portInfos.size());
    if (portInfoArray->size > 0)
    {
        portInfoArray->portInfo = new SerialPortInfo[portInfoArray->size];
        for (unsigned int i = 0; i < portInfoArray->size; ++i)
        {
            itas109::IUtils::strncpy(portInfoArray->portInfo[i].portName, portInfos[i].portName, 256);
            itas109::IUtils::strncpy(portInfoArray->portInfo[i].description, portInfos[i].description, 256);
            itas109::IUtils::strncpy(portInfoArray->portInfo[i].hardwareId, portInfos[i].hardwareId, 256);
        }
    }
    else
    {
        portInfoArray->portInfo = nullptr;
    }
}

void CSerialPortAvailablePortInfosFree(struct SerialPortInfoArray *portInfoArray)
{
    if (portInfoArray)
    {
        delete[] portInfoArray->portInfo;
        portInfoArray->portInfo = nullptr;
        portInfoArray->size = 0;
    }
}

void *CSerialPortMalloc()
{
    return new itas109::CSerialPort();
}

void CSerialPortFree(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        delete pCSP;
        pCSP = nullptr;
    }
}

void CSerialPortInit(void *pSerialPort,
                     const char *portName,
                     int baudRate,
                     Parity parity,
                     DataBits dataBits,
                     StopBits stopbits,
                     FlowControl flowControl,
                     unsigned int readBufferSize)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->init(portName, baudRate, itas109::Parity(parity), itas109::DataBits(dataBits), itas109::StopBits(stopbits), itas109::FlowControl(flowControl), readBufferSize);
    }
}

void CSerialPortSetOperateMode(void *pSerialPort, OperateMode operateMode)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->setOperateMode(itas109::OperateMode(operateMode));
    }
}

int CSerialPortOpen(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return pCSP->open();
    }

    return 0;
}

void CSerialPortClose(void *pSerialPort)
{

    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->close();
    }
}

int CSerialPortIsOpen(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return pCSP->isOpen();
    }

    return 0;
}

int CSerialPortConnectReadEvent(void *pSerialPort, pFunReadEvent pFun)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        // TODO: delete new CSPListener
        CSPListener *listen = new CSPListener(pSerialPort, pFun);
        return pCSP->connectReadEvent(listen);
    }

    return -1;
}

int CSerialPortDisconnectReadEvent(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return pCSP->disconnectReadEvent();
    }

    return -1;
}

unsigned int CSerialPortGetReadBufferUsedLen(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return pCSP->getReadBufferUsedLen();
    }

    return 0;
}

int CSerialPortReadData(void *pSerialPort, void *data, int size)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return pCSP->readData(data, size);
    }

    return -1;
}

int CSerialPortReadAllData(void *pSerialPort, void *data)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return pCSP->readAllData(data);
    }

    return -1;
}

int CSerialPortReadLineData(void *pSerialPort, void *data, int size)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return pCSP->readLineData(data, size);
    }

    return -1;
}

int CSerialPortWriteData(void *pSerialPort, const void *data, int size)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return pCSP->writeData(data, size);
    }

    return -1;
}

void CSerialPortSetDebugModel(void *pSerialPort, int isDebug)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->setDebugModel(isDebug);
    }
}

void CSerialPortSetReadIntervalTimeout(void *pSerialPort, unsigned int msecs)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->setReadIntervalTimeout(msecs);
    }
}

unsigned int CSerialPortGetReadIntervalTimeout(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return pCSP->getReadIntervalTimeout();
    }

    return 0;
}

void CSerialPortSetMinByteReadNotify(void *pSerialPort, unsigned int minByteReadNotify)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->setMinByteReadNotify(minByteReadNotify);
    }
}

int CSerialPortFlushBuffers(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->flushBuffers();
    }

    return 0;
}

int CSerialPortFlushReadBuffers(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->flushReadBuffers();
    }

    return 0;
}

int CSerialPortFlushWriteBuffers(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return pCSP->flushWriteBuffers();
    }

    return 0;
}

int CSerialPortGetLastError(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return pCSP->getLastError();
    }

    return 0;
}

const char *CSerialPortGetLastErrorMsg(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return pCSP->getLastErrorMsg();
    }

    return "";
}

void CSerialPortClearError(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->clearError();
    }
}

void CSerialPortSetPortName(void *pSerialPort, const char *portName)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->setPortName(portName);
    }
}

const char *CSerialPortGetPortName(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return pCSP->getPortName();
    }

    return "";
}

void CSerialPortSetBaudRate(void *pSerialPort, int baudRate)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->setBaudRate(baudRate);
    }
}

int CSerialPortGetBaudRate(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return pCSP->getBaudRate();
    }

    return 0;
}

void CSerialPortSetParity(void *pSerialPort, Parity parity)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->setParity(itas109::Parity(parity));
    }
}

Parity CSerialPortGetParity(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return Parity(pCSP->getParity());
    }

    return ParityNone;
}

void CSerialPortSetDataBits(void *pSerialPort, DataBits dataBits)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->setDataBits(itas109::DataBits(dataBits));
    }
}

DataBits CSerialPortGetDataBits(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return DataBits(pCSP->getDataBits());
    }

    return DataBits8;
}

void CSerialPortSetStopBits(void *pSerialPort, StopBits stopbits)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->setStopBits(itas109::StopBits(stopbits));
    }
}

StopBits CSerialPortGetStopBits(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return StopBits(pCSP->getStopBits());
    }

    return StopOne;
}

void CSerialPortSetFlowControl(void *pSerialPort, FlowControl flowControl)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->setFlowControl(itas109::FlowControl(flowControl));
    }
}

FlowControl CSerialPortGetFlowControl(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return FlowControl(pCSP->getFlowControl());
    }

    return FlowNone;
}

void CSerialPortSetReadBufferSize(void *pSerialPort, unsigned int size)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->setReadBufferSize(size);
    }
}

unsigned int CSerialPortGetReadBufferSize(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return pCSP->getReadBufferSize();
    }

    return 0;
}

void CSerialPortSetDtr(void *pSerialPort, int set)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->setDtr(set);
    }
}

void CSerialPortSetRts(void *pSerialPort, int set)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        pCSP->setRts(set);
    }
}

const char *CSerialPortGetVersion(void *pSerialPort)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(pSerialPort);
    if (pCSP)
    {
        return pCSP->getVersion();
    }

    return "";
}
