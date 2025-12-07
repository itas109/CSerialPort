#include "cserialport.h"

#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
#include "CSerialPort/IProtocolParser.h"
#include "CSerialPort/iutils.hpp"

#include <vector>
#include <cstdlib>  // malloc free
#include <string.h> // memcpy memset

class CSPReadEventListener : public itas109::CSerialPortListener
{
public:
    CSPReadEventListener(i_handle_t handle, pFunReadEvent pFun)
        : m_handle(handle)
        , m_pFun(pFun)
    {
    }

    ~CSPReadEventListener()
    {
    }

    void onReadEvent(const char *portName, unsigned int readBufferLen)
    {
        m_pFun(m_handle, portName, readBufferLen);
    }

private:
    i_handle_t m_handle;
    pFunReadEvent m_pFun;
};

class CSPHotPlugEventListener : public itas109::CSerialPortHotPlugListener
{
public:
    CSPHotPlugEventListener(i_handle_t handle, pFunHotPlugEvent pFun)
        : m_handle(handle)
        , m_pFun(pFun)
    {
    }

    ~CSPHotPlugEventListener()
    {
    }

    void onHotPlugEvent(const char *portName, int isAdd)
    {
        m_pFun(m_handle, portName, isAdd);
    }

private:
    i_handle_t m_handle;
    pFunHotPlugEvent m_pFun;
};

class CSPProtocolParser : public itas109::IProtocolParser
{
public:
    CSPProtocolParser(i_handle_t handle, pFunProtocolParser pParser, pFunProtocolEvent pFun)
        : m_handle(handle)
        , m_pParser(pParser)
        , m_pFun(pFun)
    {
    }

    ~CSPProtocolParser()
    {
    }

    unsigned int parse(const void *buffer, unsigned int size, std::vector<itas109::IProtocolResult> &results)
    {
        ProtocolResultArray resultArray;
        resultArray.result = NULL;
        resultArray.size = 0;

        unsigned int skipSize = 0;
        skipSize = m_pParser(m_handle, buffer, size, &resultArray);

        for (unsigned int i = 0; i < resultArray.size; ++i)
        {
            // TODO: no copy
            ProtocolResult resultC = resultArray.result[i];
            itas109::IProtocolResult result(resultC.data, resultC.len);
            results.push_back(result);
        }

        // free
        free(resultArray.result);

        return skipSize;
    }

    void onProtocolEvent(std::vector<itas109::IProtocolResult> &results)
    {
        for (size_t i = 0; i < results.size(); ++i)
        {
            struct ProtocolResult* result = (struct ProtocolResult *)malloc(sizeof(struct ProtocolResult));
            if (NULL != result)
            {
                // TODO: no copy
                memcpy(result->data, results.at(i).data, results.at(i).len);
                result->len = results.at(i).len;
                m_pFun(m_handle, result);
            }
        }
    }

private:
    i_handle_t m_handle;
    pFunProtocolParser m_pParser;
    pFunProtocolEvent m_pFun;
};

void CSerialPortAvailablePortInfosMalloc(SerialPortInfoArray *portInfoArray)
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
        portInfoArray->portInfo = NULL;
    }
}

void CSerialPortAvailablePortInfosFree(struct SerialPortInfoArray *portInfoArray)
{
    if (portInfoArray)
    {
        delete[] portInfoArray->portInfo;
        portInfoArray->portInfo = NULL;
        portInfoArray->size = 0;
    }
}

i_handle_t CSerialPortMalloc()
{
    return reinterpret_cast<i_handle_t>(new itas109::CSerialPort());
}

void CSerialPortFree(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        delete pCSP;
        pCSP = NULL;
    }
}

void CSerialPortInit(i_handle_t handle,
                     const char *portName,
                     int baudRate,
                     Parity parity,
                     DataBits dataBits,
                     StopBits stopbits,
                     FlowControl flowControl,
                     unsigned int readBufferSize)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->init(portName, baudRate, itas109::Parity(parity), itas109::DataBits(dataBits), itas109::StopBits(stopbits), itas109::FlowControl(flowControl), readBufferSize);
    }
}

void CSerialPortSetOperateMode(i_handle_t handle, OperateMode operateMode)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->setOperateMode(itas109::OperateMode(operateMode));
    }
}

int CSerialPortOpen(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return pCSP->open();
    }

    return 0;
}

void CSerialPortClose(i_handle_t handle)
{

    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->close();
    }
}

int CSerialPortIsOpen(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return pCSP->isOpen();
    }

    return 0;
}

int CSerialPortConnectReadEvent(i_handle_t handle, pFunReadEvent pFun)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        // TODO: delete new CSPReadEventListener
        CSPReadEventListener *listen = new CSPReadEventListener(handle, pFun);
        return pCSP->connectReadEvent(listen);
    }

    return -1;
}

int CSerialPortDisconnectReadEvent(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return pCSP->disconnectReadEvent();
    }

    return -1;
}

int CSerialPortConnectHotPlugEvent(i_handle_t handle, pFunHotPlugEvent pFun)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        // TODO: delete new CSPHotPlugEventListener
        CSPHotPlugEventListener *listen = new CSPHotPlugEventListener(handle, pFun);
        return pCSP->connectHotPlugEvent(listen);
    }

    return -1;
}

int CSerialPortDisconnectHotPlugEvent(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return pCSP->disconnectHotPlugReadEvent();
    }

    return -1;
}

int CSerialPortSetProtocolParser(i_handle_t handle, pFunProtocolParser pParser, pFunProtocolEvent pFun)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        // TODO: delete new CSPCommonProtocolParser
        CSPProtocolParser *parser = new CSPProtocolParser(handle, pParser, pFun);
        return pCSP->setProtocolParser(parser);
    }

    return -1;
}

unsigned int CSerialPortGetReadBufferUsedLen(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return pCSP->getReadBufferUsedLen();
    }

    return 0;
}

int CSerialPortReadData(i_handle_t handle, void *data, int size)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return pCSP->readData(data, size);
    }

    return -1;
}

int CSerialPortReadAllData(i_handle_t handle, void *data)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return pCSP->readAllData(data);
    }

    return -1;
}

int CSerialPortWriteData(i_handle_t handle, const void *data, int size)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return pCSP->writeData(data, size);
    }

    return -1;
}

void CSerialPortSetDebugModel(i_handle_t handle, int isDebug)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->setDebugModel(isDebug);
    }
}

void CSerialPortSetReadIntervalTimeout(i_handle_t handle, unsigned int msecs)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->setReadIntervalTimeout(msecs);
    }
}

unsigned int CSerialPortGetReadIntervalTimeout(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return pCSP->getReadIntervalTimeout();
    }

    return 0;
}

void CSerialPortSetMinByteReadNotify(i_handle_t handle, unsigned int minByteReadNotify)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->setMinByteReadNotify(minByteReadNotify);
    }
}

int CSerialPortFlushBuffers(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->flushBuffers();
    }

    return 0;
}

int CSerialPortFlushReadBuffers(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->flushReadBuffers();
    }

    return 0;
}

int CSerialPortFlushWriteBuffers(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return pCSP->flushWriteBuffers();
    }

    return 0;
}

int CSerialPortGetLastError(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return pCSP->getLastError();
    }

    return 0;
}

const char *CSerialPortGetLastErrorMsg(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return pCSP->getLastErrorMsg();
    }

    return "";
}

void CSerialPortClearError(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->clearError();
    }
}

void CSerialPortSetPortName(i_handle_t handle, const char *portName)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->setPortName(portName);
    }
}

const char *CSerialPortGetPortName(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return pCSP->getPortName();
    }

    return "";
}

void CSerialPortSetBaudRate(i_handle_t handle, int baudRate)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->setBaudRate(baudRate);
    }
}

int CSerialPortGetBaudRate(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return pCSP->getBaudRate();
    }

    return 0;
}

void CSerialPortSetParity(i_handle_t handle, Parity parity)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->setParity(itas109::Parity(parity));
    }
}

Parity CSerialPortGetParity(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return Parity(pCSP->getParity());
    }

    return ParityNone;
}

void CSerialPortSetDataBits(i_handle_t handle, DataBits dataBits)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->setDataBits(itas109::DataBits(dataBits));
    }
}

DataBits CSerialPortGetDataBits(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return DataBits(pCSP->getDataBits());
    }

    return DataBits8;
}

void CSerialPortSetStopBits(i_handle_t handle, StopBits stopbits)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->setStopBits(itas109::StopBits(stopbits));
    }
}

StopBits CSerialPortGetStopBits(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return StopBits(pCSP->getStopBits());
    }

    return StopOne;
}

void CSerialPortSetFlowControl(i_handle_t handle, FlowControl flowControl)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->setFlowControl(itas109::FlowControl(flowControl));
    }
}

FlowControl CSerialPortGetFlowControl(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return FlowControl(pCSP->getFlowControl());
    }

    return FlowNone;
}

void CSerialPortSetReadBufferSize(i_handle_t handle, unsigned int size)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->setReadBufferSize(size);
    }
}

unsigned int CSerialPortGetReadBufferSize(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return pCSP->getReadBufferSize();
    }

    return 0;
}

void CSerialPortSetDtr(i_handle_t handle, int set)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->setDtr(set);
    }
}

void CSerialPortSetRts(i_handle_t handle, int set)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        pCSP->setRts(set);
    }
}

const char *CSerialPortGetVersion(i_handle_t handle)
{
    itas109::CSerialPort *pCSP = reinterpret_cast<itas109::CSerialPort *>(handle);
    if (pCSP)
    {
        return pCSP->getVersion();
    }

    return "";
}
