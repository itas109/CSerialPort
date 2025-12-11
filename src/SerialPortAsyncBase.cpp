#include "CSerialPort/SerialPortAsyncBase.h"

#include "CSerialPort/ithread.hpp"
#include "CSerialPort/ibuffer.hpp"
#include "CSerialPort/itimer.hpp"
#include "CSerialPort/ilog.hpp"
#include "CSerialPort/IProtocolParser.h"
#include "CSerialPort/SerialPortHotPlug.hpp"

CSerialPortAsyncBase::CSerialPortAsyncBase()
    : CSerialPortAsyncBase("")
{
}

CSerialPortAsyncBase::CSerialPortAsyncBase(const char *portName)
    : CSerialPortBase(portName)
    , m_isEnableReadThread(true)
    , m_readIntervalTimeoutMS(0)
    , m_minByteReadNotify(1)
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

void CSerialPortAsyncBase::readThreadFun()
{
    unsigned int readBufferSize = 0;

    int isNew = 0;
    char dataArray[4096];
    char bufferArray[4096];
    for (; m_isEnableReadThread;)
    {
        if (waitCommEventNative())
        {
            readBufferSize = getReadBufferUsedLenNative();
            if (readBufferSize >= m_minByteReadNotify)
            {
                char *data = nullptr;
                if (readBufferSize <= 4096)
                {
                    data = dataArray;
                    isNew = 0;
                }
                else
                {
                    data = new char[readBufferSize];
                    isNew = 1;
                }

                if (data)
                {
                    if (p_readBuffer)
                    {
                        int len = readDataNative(data, readBufferSize);
                        p_readBuffer->write(data, len);
#ifdef CSERIALPORT_DEBUG
                        char hexStr[201]; // 100*2 + 1
                        LOG_INFO("%s write buffer(usedLen %u). len: %d, hex(top100): %s", m_portName, p_readBuffer->getUsedLen(), len, itas109::IUtils::charToHexStr(hexStr, data, len > 100 ? 100 : len));
#endif

                        if (p_protocolParser)
                        {
                            int realSize = p_readBuffer->peek(bufferArray, 4096);

                            unsigned int skipSize = 0;
                            std::vector<itas109::IProtocolResult> results;
                            skipSize = p_protocolParser->parse(bufferArray, realSize, results);

                            p_readBuffer->skip(skipSize);

                            if (!results.empty())
                            {
                                p_protocolParser->onProtocolEvent(results);
                            }
                        }
                        else
                        {
                            if (p_readEvent)
                            {
                                if (m_readIntervalTimeoutMS > 0)
                                {
                                    if (p_timer)
                                    {
                                        if (p_timer->isRunning())
                                        {
                                            p_timer->stop();
                                        }

                                        if (p_readBuffer->isFull() || p_readBuffer->getUsedLen() > getByteReadBufferFullNotify())
                                        {
                                            LOG_INFO("onReadEvent buffer full. portName: %s, readLen: %u", getPortName(), p_readBuffer->getUsedLen());
                                            p_readEvent->onReadEvent(getPortName(), p_readBuffer->getUsedLen());
                                        }
                                        else
                                        {
                                            p_timer->startOnce(m_readIntervalTimeoutMS, p_readEvent, &itas109::CSerialPortListener::onReadEvent, getPortName(), p_readBuffer->getUsedLen());
                                        }
                                    }
                                }
                                else
                                {
                                    LOG_INFO("onReadEvent min read byte. portName: %s, readLen: %u", getPortName(), p_readBuffer->getUsedLen());
                                    p_readEvent->onReadEvent(getPortName(), p_readBuffer->getUsedLen());
                                }
                            }
                        }
                    }

                    if (isNew)
                    {
                        delete[] data;
                        data = nullptr;
                    }
                }
            }
        }
    }
}

bool CSerialPortAsyncBase::startReadThread()
{
    // start read thread
    bool bRet = false;
    try
    {
        m_readThread = std::thread(&CSerialPortAsyncBase::readThreadFun, this);
        bRet = true;
    }
    catch (...)
    {
        bRet = false;
    }

#ifdef CSERIALPORT_DEBUG
    LOG_INFO("%s start read thread %s. thread id: %u", m_portName, bRet ? "success" : "failed", (*(unsigned int*)&m_readThread.get_id()));
#endif

    return bRet;
}

bool CSerialPortAsyncBase::stopReadThread()
{
#ifdef CSERIALPORT_DEBUG
    LOG_INFO("%s stop read thread...", m_portName);
#endif

    m_isEnableReadThread = false;

    beforeStopReadThread();

    if (m_readThread.joinable())
    {
        m_readThread.join();
    }

#ifdef CSERIALPORT_DEBUG
    LOG_INFO("%s stop read thread success", m_portName);
#endif

    return true;
}