#include "CSerialPort/SerialPortWinBase.h"
#include "CSerialPort/SerialPortListener.h"
#include "CSerialPort/IProtocolParser.h"
#include "CSerialPort/iutils.hpp"
#include "CSerialPort/ithread.hpp"
#include "CSerialPort/itimer.hpp"
#include "CSerialPort/ilog.hpp"

#ifdef UNICODE
static wchar_t *CharToWChar(wchar_t *dest, const char *str)
{
    if (nullptr == str)
    {
        return nullptr;
    }

    int len = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0); // get char length
    MultiByteToWideChar(CP_ACP, 0, str, -1, dest, len);            // CP_UTF8

    return dest;
}
#endif

CSerialPortWinBase::CSerialPortWinBase()
    : CSerialPortWinBase("")
{
}

CSerialPortWinBase::CSerialPortWinBase(const char *portName)
    : CSerialPortAsyncBase(portName)
    , m_baudRate(itas109::BaudRate9600)
    , m_parity(itas109::ParityNone)
    , m_dataBits(itas109::DataBits8)
    , m_stopbits(itas109::StopOne)
    , m_flowControl(itas109::FlowNone)
    , m_readBufferSize(4096)
    , m_handle(INVALID_HANDLE_VALUE)
    , m_overlapMonitor()
    , m_overlapRead()
    , m_overlapWrite()
    , m_comConfigure()
    , m_comTimeout()
    , m_communicationMutex()
    , m_isThreadRunning(false)
    , p_buffer(new itas109::RingBuffer<char>(m_readBufferSize))
{
    itas109::IUtils::strncpy(m_portName, portName, 256);
    m_byteReadBufferFullNotify = (unsigned int)(m_readBufferSize * 0.8);

    m_overlapMonitor.Internal = 0;
    m_overlapMonitor.InternalHigh = 0;
    m_overlapMonitor.Offset = 0;
    m_overlapMonitor.OffsetHigh = 0;
    m_overlapMonitor.hEvent = CreateEvent(nullptr, true, false, nullptr);
}

CSerialPortWinBase::~CSerialPortWinBase()
{
    CloseHandle(m_overlapMonitor.hEvent);

    if (p_buffer)
    {
        delete p_buffer;
        p_buffer = nullptr;
    }
}

void CSerialPortWinBase::init(const char *portName,
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
    m_byteReadBufferFullNotify = (unsigned int)(m_readBufferSize * 0.8);

    if (p_buffer)
    {
        delete p_buffer;
        p_buffer = nullptr;
    }
    p_buffer = new itas109::RingBuffer<char>(m_readBufferSize);
}

bool CSerialPortWinBase::openPort()
{
    itas109::IScopedLock lock(m_mutex);

    LOG_INFO("portName: %s, baudRate: %d, dataBit: %d, parity: %d, stopBit: %d, flowControl: %d, mode: %s, readBufferSize:%u(%u), readIntervalTimeoutMS: %u, minByteReadNotify: %u, byteReadBufferFullNotify: %u",
             m_portName, m_baudRate, m_dataBits, m_parity, m_stopbits, m_flowControl, m_operateMode == itas109::AsynchronousOperate ? "async" : "sync", m_readBufferSize, p_buffer->getBufferSize(), m_readIntervalTimeoutMS, m_minByteReadNotify, m_byteReadBufferFullNotify);

    bool bRet = false;

    TCHAR *tcPortName = nullptr;
    char portName[256] = "\\\\.\\";                    // support COM10 above \\\\.\\COM10
    itas109::IUtils::strncat(portName, m_portName, 7); // COM254

#ifdef UNICODE
    wchar_t wstr[256];
    tcPortName = CharToWChar(wstr, portName);
#else
    tcPortName = portName;
#endif
    unsigned long configSize = sizeof(COMMCONFIG);
    m_comConfigure.dwSize = configSize;

    DWORD dwFlagsAndAttributes = 0;
    if (m_operateMode == itas109::/*OperateMode::*/ AsynchronousOperate)
    {
        dwFlagsAndAttributes += FILE_FLAG_OVERLAPPED;
    }

    if (!isOpen())
    {
        // get a handle to the port
        m_handle = CreateFile(tcPortName,                   // communication port string (COMX)
                              GENERIC_READ | GENERIC_WRITE, // read/write types
                              0,                            // comm devices must be opened with exclusive access
                              nullptr,                      // no security attributes
                              OPEN_EXISTING,                // comm devices must use OPEN_EXISTING
                              dwFlagsAndAttributes,         // Async I/O or sync I/O
                              nullptr);

        if (m_handle != INVALID_HANDLE_VALUE)
        {
            // set system internal input output buffer size
            SetupComm(m_handle, m_readBufferSize <= 4096 ? 4096 : m_readBufferSize, m_readBufferSize <= 4096 ? 4096 : m_readBufferSize); // windows default 4096

            // get default parameter
            GetCommConfig(m_handle, &m_comConfigure, &configSize);
            GetCommState(m_handle, &(m_comConfigure.dcb));

            // set parameter
            m_comConfigure.dcb.BaudRate = m_baudRate;
            m_comConfigure.dcb.ByteSize = m_dataBits; // Number of bits/byte, 4-8
            m_comConfigure.dcb.Parity = m_parity;     // 0-4=None,Odd,Even,Mark,Space
            m_comConfigure.dcb.StopBits = m_stopbits; // 0,1,2 = 1, 1.5, 2
            switch (m_flowControl)
            {
                case itas109::/*FlowControl::*/ FlowNone: // No flow control

                    m_comConfigure.dcb.fOutxCtsFlow = FALSE;
                    m_comConfigure.dcb.fRtsControl = RTS_CONTROL_DISABLE;
                    m_comConfigure.dcb.fInX = FALSE;
                    m_comConfigure.dcb.fOutX = FALSE;
                    break;

                case itas109::/*FlowControl::*/ FlowSoftware: // Software(XON / XOFF) flow control
                    m_comConfigure.dcb.fOutxCtsFlow = FALSE;
                    m_comConfigure.dcb.fRtsControl = RTS_CONTROL_DISABLE;
                    m_comConfigure.dcb.fInX = TRUE;
                    m_comConfigure.dcb.fOutX = TRUE;
                    break;

                case itas109::/*FlowControl::*/ FlowHardware: // Hardware(RTS / CTS) flow control
                    m_comConfigure.dcb.fOutxCtsFlow = TRUE;
                    m_comConfigure.dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
                    m_comConfigure.dcb.fInX = FALSE;
                    m_comConfigure.dcb.fOutX = FALSE;
                    break;
            }

            m_comConfigure.dcb.fBinary = true;
            m_comConfigure.dcb.fInX = false;
            m_comConfigure.dcb.fOutX = false;
            m_comConfigure.dcb.fAbortOnError = false;
            m_comConfigure.dcb.fNull = false;

            if (SetCommConfig(m_handle, &m_comConfigure, configSize))
            {
                // @todo
                // Discards all characters from the output or input buffer of a specified communications resource. It
                // can also terminate pending read or write operations on the resource.
                PurgeComm(m_handle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_RXABORT);

                // init event driven approach
                if (m_operateMode == itas109::/*OperateMode::*/ AsynchronousOperate)
                {
                    m_comTimeout.ReadIntervalTimeout = MAXDWORD;
                    m_comTimeout.ReadTotalTimeoutMultiplier = 0;
                    m_comTimeout.ReadTotalTimeoutConstant = 0;
                    m_comTimeout.WriteTotalTimeoutMultiplier = 0;
                    m_comTimeout.WriteTotalTimeoutConstant = 0;
                    SetCommTimeouts(m_handle, &m_comTimeout);

                    // set comm event
                    // only need receive event
                    if (SetCommMask(m_handle, EV_RXCHAR))
                    {
                        m_isThreadRunning = true;
                        bRet = startThreadMonitor();

                        if (bRet)
                        {
                            m_lastError = itas109::/*SerialPortError::*/ ErrorOK;
                        }
                        else
                        {
                            m_isThreadRunning = false;
                            m_lastError = itas109::/*SerialPortError::*/ ErrorInner;
                        }
                    }
                    else
                    {
                        // Failed to set Comm Mask
                        bRet = false;
                        m_lastError = itas109::/*SerialPortError::*/ ErrorInvalidParam;
                    }
                }
                else
                {
                    m_comTimeout.ReadIntervalTimeout = MAXDWORD;
                    m_comTimeout.ReadTotalTimeoutMultiplier = 0;
                    m_comTimeout.ReadTotalTimeoutConstant = 0;
                    m_comTimeout.WriteTotalTimeoutMultiplier = 100;
                    m_comTimeout.WriteTotalTimeoutConstant = 500;
                    SetCommTimeouts(m_handle, &m_comTimeout);

                    bRet = true;
                    m_lastError = itas109::/*SerialPortError::*/ ErrorOK;
                }
            }
            else
            {
                // set com configure error
                bRet = false;
                m_lastError = itas109::/*SerialPortError::*/ ErrorInvalidParam;
            }
        }
        else
        {
            // 串口打开失败，增加提示信息
            switch (GetLastError())
            {
                // 串口不存在
                case ERROR_FILE_NOT_FOUND:
                {
                    m_lastError = itas109::/*SerialPortError::*/ ErrorNotExist;

                    break;
                }
                    // 串口拒绝访问
                case ERROR_ACCESS_DENIED:
                {
                    m_lastError = itas109::/*SerialPortError::*/ ErrorAccessDenied;

                    break;
                }
                default:
                    m_lastError = itas109::/*SerialPortError::*/ ErrorOpenFailed;
                    break;
            }
        }
    }
    else
    {
        bRet = false;
        m_lastError = itas109::/*SerialPortError::*/ ErrorOpenFailed;
    }

    if (!bRet)
    {
        closePort();
    }

    LOG_INFO("open %s. code: %d, message: %s", m_portName, getLastError(), getLastErrorMsg());

    return bRet;
}

void CSerialPortWinBase::closePort()
{
    if (isOpen())
    {
        stopThreadMonitor();

        if (INVALID_HANDLE_VALUE != m_handle)
        {
            // Discards all characters from the output or input buffer of a specified communications resource. It can
            // also terminate pending read or write operations on the resource.
            PurgeComm(m_handle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_RXABORT);

            CloseHandle(m_handle);
            m_handle = INVALID_HANDLE_VALUE;
        }

        ResetEvent(m_overlapMonitor.hEvent);
    }
}

void CSerialPortWinBase::commThreadMonitor()
{
    DWORD dwError = 0;
    COMSTAT comstat;
    DWORD eventMask = 0;

    int isNew = 0;
    char dataArray[4096];
    char bufferArray[4096];
    for (; m_isThreadRunning;)
    {
        eventMask = 0;
        if (!WaitCommEvent(m_handle, &eventMask, &m_overlapMonitor))
        {
            if (ERROR_IO_PENDING == GetLastError())
            {
                WaitForSingleObject(m_overlapMonitor.hEvent, INFINITE);
            }
        }

        if (eventMask & EV_RXCHAR)
        {
            ClearCommError(m_handle, &dwError, &comstat);
            if (comstat.cbInQue >= m_minByteReadNotify)
            {
                char *data = nullptr;
                if (comstat.cbInQue <= 4096)
                {
                    data = dataArray;
                    isNew = 0;
                }
                else
                {
                    data = new char[comstat.cbInQue];
                    isNew = 1;
                }

                if (data)
                {
                    if (p_buffer)
                    {
                        int len = readDataWin(data, comstat.cbInQue);
                        p_buffer->write(data, len);
#ifdef CSERIALPORT_DEBUG
                        char hexStr[201]; // 100*2 + 1
                        LOG_INFO("write buffer(usedLen %u). len: %d, hex(top100): %s", p_buffer->getUsedLen(), len, itas109::IUtils::charToHexStr(hexStr, data, len > 100 ? 100 : len));
#endif

                        if (p_protocolParser)
                        {
                            int realSize = p_buffer->peek(bufferArray, 4096);

                            unsigned int skipSize = 0;
                            std::vector<itas109::IProtocolResult> results;
                            skipSize = p_protocolParser->parse(bufferArray, realSize, results);

                            p_buffer->skip(skipSize);

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

                                        if (p_buffer->isFull() || p_buffer->getUsedLen() > getByteReadBufferFullNotify())
                                        {
                                            LOG_INFO("onReadEvent buffer full. portName: %s, readLen: %u", getPortName(), p_buffer->getUsedLen());
                                            p_readEvent->onReadEvent(getPortName(), p_buffer->getUsedLen());
                                        }
                                        else
                                        {
                                            p_timer->startOnce(m_readIntervalTimeoutMS, p_readEvent, &itas109::CSerialPortListener::onReadEvent, getPortName(), p_buffer->getUsedLen());
                                        }
                                    }
                                }
                                else
                                {
                                    LOG_INFO("onReadEvent min read byte. portName: %s, readLen: %u", getPortName(), p_buffer->getUsedLen());
                                    p_readEvent->onReadEvent(getPortName(), p_buffer->getUsedLen());
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

bool CSerialPortWinBase::isOpen()
{
    return m_handle != INVALID_HANDLE_VALUE;
}

unsigned int CSerialPortWinBase::getReadBufferUsedLen()
{
    unsigned int usedLen = 0;

    if (isOpen())
    {
        if (m_operateMode == itas109::/*OperateMode::*/ AsynchronousOperate)
        {
            usedLen = p_buffer->getUsedLen();
        }
        else
        {
            DWORD dwError = 0;
            COMSTAT comstat;
            ClearCommError(m_handle, &dwError, &comstat);
            usedLen = comstat.cbInQue;
        }

#ifdef CSERIALPORT_DEBUG
        if (usedLen > 0)
        {
            LOG_INFO("getReadBufferUsedLen: %u", usedLen);
        }
#endif
    }

    return usedLen;
}

int CSerialPortWinBase::readDataWin(void *data, int size)
{
    itas109::IScopedLock lock(m_mutexRead);

    DWORD numBytes = 0;

    if (isOpen())
    {
        if (m_operateMode == itas109::/*OperateMode::*/ AsynchronousOperate)
        {
            m_overlapRead.Internal = 0;
            m_overlapRead.InternalHigh = 0;
            m_overlapRead.Offset = 0;
            m_overlapRead.OffsetHigh = 0;
            m_overlapRead.hEvent = CreateEvent(nullptr, true, false, nullptr);

            if (!ReadFile(m_handle, (void *)data, (DWORD)size, &numBytes, &m_overlapRead))
            {
                if (ERROR_IO_PENDING == GetLastError())
                {
                    GetOverlappedResult(m_handle, &m_overlapRead, &numBytes, true);
                }
                else
                {
                    m_lastError = itas109::/*SerialPortError::*/ ErrorReadFailed;
                    numBytes = (DWORD)-1;
                }
            }

            CloseHandle(m_overlapRead.hEvent);
        }
        else
        {
            if (ReadFile(m_handle, (void *)data, (DWORD)size, &numBytes, nullptr))
            {
            }
            else
            {
                m_lastError = itas109::/*SerialPortError::*/ ErrorReadFailed;
                numBytes = (DWORD)-1;
            }
        }
    }
    else
    {
        m_lastError = itas109::/*SerialPortError::*/ ErrorNotOpen;
        numBytes = (DWORD)-1;
    }

    return numBytes;
}

int CSerialPortWinBase::readData(void *data, int size)
{
    itas109::IScopedLock lock(m_mutexRead);

    if (size <= 0)
    {
        return 0;
    }

    DWORD numBytes = 0;

    if (isOpen())
    {
        if (m_operateMode == itas109::/*OperateMode::*/ AsynchronousOperate)
        {
            numBytes = p_buffer->read((char *)data, size);
        }
        else
        {
            if (ReadFile(m_handle, data, (DWORD)size, &numBytes, nullptr))
            {
            }
            else
            {
                m_lastError = itas109::/*SerialPortError::*/ ErrorReadFailed;
                numBytes = (DWORD)-1;
            }
        }
    }
    else
    {
        m_lastError = itas109::/*SerialPortError::*/ ErrorNotOpen;
        numBytes = (DWORD)-1;
    }

#ifdef CSERIALPORT_DEBUG
    char hexStr[201]; // 100*2 + 1
    LOG_INFO("read. len: %d, hex(top100): %s", numBytes, itas109::IUtils::charToHexStr(hexStr, (const char *)data, numBytes > 100 ? 100 : numBytes));
#endif

    return numBytes;
}

int CSerialPortWinBase::readAllData(void *data)
{
    return readData(data, getReadBufferUsedLen());
}

int CSerialPortWinBase::readLineData(void *data, int size)
{
    itas109::IScopedLock lock(m_mutexRead);

    DWORD numBytes = 0;

    if (isOpen())
    {
        m_lastError = itas109::/*SerialPortError::*/ ErrorNotImplemented;
        numBytes = (DWORD)-1;
    }
    else
    {
        m_lastError = itas109::/*SerialPortError::*/ ErrorNotOpen;
        numBytes = (DWORD)-1;
    }

    return numBytes;
}

int CSerialPortWinBase::writeData(const void *data, int size)
{
    itas109::IScopedLock lock(m_mutexWrite);

    DWORD numBytes = 0;

    if (isOpen())
    {
        // @todo maybe mutile thread not need this
        // Discards all characters from the output or input buffer of a specified communications resource. It can also
        // terminate pending read or write operations on the resource.
        //::PurgeComm(m_handle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_RXABORT);

#ifdef CSERIALPORT_DEBUG
        char hexStr[201]; // 100*2 + 1
        LOG_INFO("write. len: %d, hex(top100): %s", size, itas109::IUtils::charToHexStr(hexStr, (const char *)data, size > 100 ? 100 : size));
#endif

        if (m_operateMode == itas109::/*OperateMode::*/ AsynchronousOperate)
        {
            m_overlapWrite.Internal = 0;
            m_overlapWrite.InternalHigh = 0;
            m_overlapWrite.Offset = 0;
            m_overlapWrite.OffsetHigh = 0;
            m_overlapWrite.hEvent = CreateEvent(nullptr, true, false, nullptr);

            if (!WriteFile(m_handle, (void *)data, (DWORD)size, &numBytes, &m_overlapWrite))
            {
                if (ERROR_IO_PENDING == GetLastError())
                {
                    GetOverlappedResult(m_handle, &m_overlapWrite, &numBytes, true);
                }
                else
                {
                    m_lastError = itas109::/*SerialPortError::*/ ErrorWriteFailed;
                    numBytes = (DWORD)-1;
                }
            }

            CloseHandle(m_overlapWrite.hEvent);
        }
        else
        {
            if (WriteFile(m_handle, (void *)data, (DWORD)size, &numBytes, nullptr))
            {
            }
            else
            {
                m_lastError = itas109::/*SerialPortError::*/ ErrorWriteFailed;
                numBytes = (DWORD)-1;
            }
        }
    }
    else
    {
        m_lastError = itas109::/*SerialPortError::*/ ErrorNotOpen;
        numBytes = (DWORD)-1;
    }

    return numBytes;
}

void CSerialPortWinBase::setDebugModel(bool isDebug)
{
    //@todo
}

void CSerialPortWinBase::setReadIntervalTimeout(unsigned int msecs)
{
    m_readIntervalTimeoutMS = msecs;
}

void CSerialPortWinBase::setMinByteReadNotify(unsigned int minByteReadNotify)
{
    m_minByteReadNotify = minByteReadNotify;
}

bool CSerialPortWinBase::flushBuffers()
{
    itas109::IScopedLock lock(m_mutex);

    if (isOpen())
    {
        return TRUE == FlushFileBuffers(m_handle);
    }
    else
    {
        return false;
    }
}

bool CSerialPortWinBase::flushReadBuffers()
{
    itas109::IScopedLock lock(m_mutex);

    if (isOpen())
    {
        return TRUE == PurgeComm(m_handle, PURGE_RXCLEAR);
    }
    else
    {
        return false;
    }
}

bool CSerialPortWinBase::flushWriteBuffers()
{
    itas109::IScopedLock lock(m_mutex);

    if (isOpen())
    {
        return TRUE == PurgeComm(m_handle, PURGE_TXCLEAR);
    }
    else
    {
        return false;
    }
}

void CSerialPortWinBase::setPortName(const char *portName)
{
    // Windows : COM1
    // Linux : /dev/ttyS0
    itas109::IUtils::strncpy(m_portName, portName, 256);
}

const char *CSerialPortWinBase::getPortName() const
{
    return m_portName;
}

void CSerialPortWinBase::setBaudRate(int baudRate)
{
    m_baudRate = baudRate;
}

int CSerialPortWinBase::getBaudRate() const
{
    return m_baudRate;
}

void CSerialPortWinBase::setParity(itas109::Parity parity)
{
    m_parity = parity;
}

itas109::Parity CSerialPortWinBase::getParity() const
{
    return m_parity;
}

void CSerialPortWinBase::setDataBits(itas109::DataBits dataBits)
{
    m_dataBits = dataBits;
}

itas109::DataBits CSerialPortWinBase::getDataBits() const
{
    return m_dataBits;
}

void CSerialPortWinBase::setStopBits(itas109::StopBits stopbits)
{
    m_stopbits = stopbits;
}

itas109::StopBits CSerialPortWinBase::getStopBits() const
{
    return m_stopbits;
}

void CSerialPortWinBase::setFlowControl(itas109::FlowControl flowControl)
{
    m_flowControl = flowControl;
}

itas109::FlowControl CSerialPortWinBase::getFlowControl() const
{
    return m_flowControl;
}

void CSerialPortWinBase::setReadBufferSize(unsigned int size)
{
    itas109::IScopedLock lock(m_mutex);
    if (isOpen())
    {
        m_readBufferSize = size;
    }
}

unsigned int CSerialPortWinBase::getReadBufferSize() const
{
    return m_readBufferSize;
}

void CSerialPortWinBase::setDtr(bool set /*= true*/)
{
    itas109::IScopedLock lock(m_mutex);
    if (isOpen())
    {
        EscapeCommFunction(m_handle, set ? SETDTR : CLRDTR);
    }
}

void CSerialPortWinBase::setRts(bool set /*= true*/)
{
    itas109::IScopedLock lock(m_mutex);
    if (isOpen())
    {
        EscapeCommFunction(m_handle, set ? SETRTS : CLRRTS);
    }
}

bool CSerialPortWinBase::startThreadMonitor()
{
    // start event thread monitor
    bool bRet = false;
    try
    {
        m_monitorThread = std::thread(&CSerialPortWinBase::commThreadMonitor, this);
        bRet = true;
    }
    catch (...)
    {
        bRet = false;
    }

    return bRet;
}

bool CSerialPortWinBase::stopThreadMonitor()
{
    m_isThreadRunning = false;

    // 唤醒等待的线程，避免join()阻塞
    if (INVALID_HANDLE_VALUE != m_handle)
    {
        SetCommMask(m_handle, 0);          // stop WaitCommEvent
        SetEvent(m_overlapMonitor.hEvent); // stop WaitForSingleObject
    }

    if (m_monitorThread.joinable())
    {
        m_monitorThread.join();
    }

    return true;
}
