#include "CSerialPort/SerialPortWinBase.h"
#include "CSerialPort/SerialPortListener.h"
#include "CSerialPort/iutils.hpp"
#include "CSerialPort/ithread.hpp"
#include "CSerialPort/itimer.hpp"

#ifdef UNICODE
static wchar_t *CharToWChar(wchar_t *dest, const char *str)
{
    if (NULL == str)
    {
        return NULL;
    }

    int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0); // get char length
    MultiByteToWideChar(CP_ACP, 0, str, -1, dest, len);         // CP_UTF8

    return dest;
}
#endif

CSerialPortWinBase::CSerialPortWinBase()
    : m_baudRate(itas109::BaudRate9600)
    , m_parity(itas109::ParityNone)
    , m_dataBits(itas109::DataBits8)
    , m_stopbits(itas109::StopOne)
    , m_flowControl(itas109::FlowNone)
    , m_readBufferSize(4096)
    , m_handle(INVALID_HANDLE_VALUE)
    , m_monitorThread(INVALID_HANDLE_VALUE)
    , overlapMonitor()
    , m_overlapRead()
    , m_overlapWrite()
    , m_comConfigure()
    , m_comTimeout()
    , m_communicationMutex()
    , m_isThreadRunning(false)
    , p_buffer(new itas109::RingBuffer<char>(m_readBufferSize))
{
    itas109::IUtils::strncpy(m_portName, "", 1);

    overlapMonitor.Internal = 0;
    overlapMonitor.InternalHigh = 0;
    overlapMonitor.Offset = 0;
    overlapMonitor.OffsetHigh = 0;
    overlapMonitor.hEvent = CreateEvent(NULL, true, false, NULL);
}

CSerialPortWinBase::CSerialPortWinBase(const char *portName)
    : m_baudRate(itas109::BaudRate9600)
    , m_parity(itas109::ParityNone)
    , m_dataBits(itas109::DataBits8)
    , m_stopbits(itas109::StopOne)
    , m_flowControl(itas109::FlowNone)
    , m_readBufferSize(4096)
    , m_handle(INVALID_HANDLE_VALUE)
    , m_monitorThread(INVALID_HANDLE_VALUE)
    , overlapMonitor()
    , m_overlapRead()
    , m_overlapWrite()
    , m_comConfigure()
    , m_comTimeout()
    , m_communicationMutex()
    , m_isThreadRunning(false)
    , p_buffer(new itas109::RingBuffer<char>(m_readBufferSize))
{
    itas109::IUtils::strncpy(m_portName, portName, 256);

    overlapMonitor.Internal = 0;
    overlapMonitor.InternalHigh = 0;
    overlapMonitor.Offset = 0;
    overlapMonitor.OffsetHigh = 0;
    overlapMonitor.hEvent = CreateEvent(NULL, true, false, NULL);
}

CSerialPortWinBase::~CSerialPortWinBase()
{
    CloseHandle(overlapMonitor.hEvent);

    if (p_buffer)
    {
        delete p_buffer;
        p_buffer = NULL;
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

    if (p_buffer)
    {
        delete p_buffer;
        p_buffer = NULL;
    }
    p_buffer = new itas109::RingBuffer<char>(m_readBufferSize);
}

bool CSerialPortWinBase::openPort()
{
    itas109::IAutoLock lock(p_mutex);

    LOG_INFO("portName: %s, baudRate: %d, dataBit: %d, parity: %d, stopBit: %d, flowControl: %d, mode: %s, readBufferSize:%u(%u), readIntervalTimeoutMS: %u, minByteReadNotify: %u",
             m_portName, m_baudRate, m_dataBits, m_parity, m_stopbits, m_flowControl, m_operateMode == itas109::AsynchronousOperate ? "async" : "sync", m_readBufferSize,
             p_buffer->getBufferSize(), m_readIntervalTimeoutMS, m_minByteReadNotify);

    bool bRet = false;

    TCHAR *tcPortName = NULL;
    char portName[256] = "\\\\.\\";                    // support COM10 above \\\\.\\COM10
    itas109::IUtils::strncat(portName, m_portName, 6); // COM254

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
                              NULL,                         // no security attributes
                              OPEN_EXISTING,                // comm devices must use OPEN_EXISTING
                              dwFlagsAndAttributes,         // Async I/O or sync I/O
                              NULL);

        if (m_handle != INVALID_HANDLE_VALUE)
        {
            // get default parameter
            GetCommConfig(m_handle, &m_comConfigure, &configSize);
            GetCommState(m_handle, &(m_comConfigure.dcb));

            // set parameter
            m_comConfigure.dcb.BaudRate = m_baudRate;
            m_comConfigure.dcb.ByteSize = m_dataBits;
            m_comConfigure.dcb.Parity = m_parity;
            m_comConfigure.dcb.StopBits = m_stopbits;
            // m_comConfigure.dcb.fDtrControl;
            // m_comConfigure.dcb.fRtsControl;

            m_comConfigure.dcb.fBinary = true;
            m_comConfigure.dcb.fInX = false;
            m_comConfigure.dcb.fOutX = false;
            m_comConfigure.dcb.fAbortOnError = false;
            m_comConfigure.dcb.fNull = false;

            // setBaudRate(m_baudRate);
            // setDataBits(m_dataBits);
            // setStopBits(m_stopbits);
            // setParity(m_parity);

            setFlowControl(m_flowControl); // @todo

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

                        if (!bRet)
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
    // Finished
    if (isOpen())
    {
        stopThreadMonitor();

        if (m_handle != INVALID_HANDLE_VALUE)
        {
            // stop all event
            SetCommMask(m_handle, 0); // SetCommMask(m_handle,0) stop WaitCommEvent()

            // Discards all characters from the output or input buffer of a specified communications resource. It can
            // also terminate pending read or write operations on the resource.
            PurgeComm(m_handle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_RXABORT);

            CloseHandle(m_handle);
            m_handle = INVALID_HANDLE_VALUE;
        }

        ResetEvent(overlapMonitor.hEvent);
    }
}

unsigned int __stdcall CSerialPortWinBase::commThreadMonitor(LPVOID pParam)
{
    // Cast the void pointer passed to the thread back to
    // a pointer of CSerialPortWinBase class
    CSerialPortWinBase *p_base = (CSerialPortWinBase *)pParam;

    int iRet = 0;

    DWORD dwError = 0;
    COMSTAT comstat;

    if (p_base)
    {
        DWORD eventMask = 0;

        HANDLE m_mainHandle = p_base->getMainHandle();
        OVERLAPPED m_overlapMonitor = p_base->getOverlapMonitor();

        for (; p_base->isThreadRunning();)
        {
            eventMask = 0;
            // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-waitcommevent
            if (!WaitCommEvent(m_mainHandle, &eventMask, &m_overlapMonitor))
            {
                if (ERROR_IO_PENDING == GetLastError())
                {
                    // method 1
                    WaitForSingleObject(m_overlapMonitor.hEvent, INFINITE);

                    // method 2
                    // DWORD numBytes;
                    // GetOverlappedResult(m_mainHandle, &m_overlapMonitor, &numBytes, TRUE);
                }
            }

            if (eventMask & EV_RXCHAR)
            {
                // solve 线程中循环的低效率问题
                ClearCommError(m_mainHandle, &dwError, &comstat);
                if (comstat.cbInQue >= p_base->getMinByteReadNotify()) // 设定字符数,默认为1
                {
                    char *data = NULL;
                    data = new char[comstat.cbInQue];
                    if (data)
                    {
                        if (p_base->p_buffer)
                        {
                            int len = p_base->readDataWin(data, comstat.cbInQue);
                            p_base->p_buffer->write(data, len);
#ifdef CSERIALPORT_DEBUG
                            char hexStr[201]; // 100*2 + 1
                            LOG_INFO("write buffer(usedLen %u). len: %d, hex(top100): %s", p_base->p_buffer->getUsedLen(), len,
                                     itas109::IUtils::charToHexStr(hexStr, data, len > 100 ? 100 : len));
#endif

                            if (p_base->p_readEvent)
                            {
                                unsigned int readIntervalTimeoutMS = p_base->getReadIntervalTimeout();
                                if (readIntervalTimeoutMS > 0)
                                {
                                    if (p_base->p_timer)
                                    {
                                        if (p_base->p_timer->isRunning())
                                        {
                                            p_base->p_timer->stop();
                                        }

                                        LOG_INFO("onReadEvent. portName: %s, readLen: %u", p_base->getPortName(), p_base->p_buffer->getUsedLen());
                                        p_base->p_timer->startOnce(readIntervalTimeoutMS, p_base->p_readEvent, &itas109::CSerialPortListener::onReadEvent, p_base->getPortName(),
                                                                   p_base->p_buffer->getUsedLen());
                                    }
                                }
                                else
                                {
                                    LOG_INFO("onReadEvent. portName: %s, readLen: %u", p_base->getPortName(), p_base->p_buffer->getUsedLen());
                                    p_base->p_readEvent->onReadEvent(p_base->getPortName(), p_base->p_buffer->getUsedLen());
                                }
                            }
                        }

                        delete[] data;
                        data = NULL;
                    }
                }
            }
        }
    }
    else
    {
        // point null
        iRet = 0;
    }

    return iRet;
}

bool CSerialPortWinBase::isOpen()
{
    // Finished
    return m_handle != INVALID_HANDLE_VALUE;
}

unsigned int CSerialPortWinBase::getReadBufferUsedLen() const
{
    unsigned int usedLen = 0;

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

    LOG_INFO("getReadBufferUsedLen: %u", usedLen);

    return usedLen;
}

int CSerialPortWinBase::readDataWin(void *data, int size)
{
    itas109::IAutoLock lock(p_mutex);

    DWORD numBytes = 0;

    if (isOpen())
    {
        if (m_operateMode == itas109::/*OperateMode::*/ AsynchronousOperate)
        {
            m_overlapRead.Internal = 0;
            m_overlapRead.InternalHigh = 0;
            m_overlapRead.Offset = 0;
            m_overlapRead.OffsetHigh = 0;
            m_overlapRead.hEvent = CreateEvent(NULL, true, false, NULL);

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
            if (ReadFile(m_handle, (void *)data, (DWORD)size, &numBytes, NULL))
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
    itas109::IAutoLock lock(p_mutex);

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
            if (ReadFile(m_handle, data, (DWORD)size, &numBytes, NULL))
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
    itas109::IAutoLock lock(p_mutex);

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
    itas109::IAutoLock lock(p_mutex);

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
            m_overlapWrite.hEvent = CreateEvent(NULL, true, false, NULL);

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
            if (WriteFile(m_handle, (void *)data, (DWORD)size, &numBytes, NULL))
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
    itas109::IAutoLock lock(p_mutex);

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
    itas109::IAutoLock lock(p_mutex);

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
    itas109::IAutoLock lock(p_mutex);

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
    itas109::IAutoLock lock(p_mutex);
    m_baudRate = baudRate;
    m_comConfigure.dcb.BaudRate = m_baudRate;
    SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
}

int CSerialPortWinBase::getBaudRate() const
{
    return m_baudRate;
}

void CSerialPortWinBase::setParity(itas109::Parity parity)
{
    itas109::IAutoLock lock(p_mutex);
    m_parity = parity;

    if (isOpen())
    {
        m_comConfigure.dcb.Parity = (unsigned char)parity;
        switch (parity)
        {
            case itas109::/*Parity::*/ ParityNone:
                m_comConfigure.dcb.fParity = FALSE;
                break;
            case itas109::/*Parity::*/ ParityOdd:
                m_comConfigure.dcb.fParity = TRUE;
                break;
            case itas109::/*Parity::*/ ParityEven:
                m_comConfigure.dcb.fParity = TRUE;
                break;
            case itas109::/*Parity::*/ ParitySpace:
                if (m_dataBits == itas109::/*DataBits::*/ DataBits8)
                {
                    // Space parity with 8 data bits is not supported by POSIX systems
                }
                m_comConfigure.dcb.fParity = TRUE;
                break;
            case itas109::/*Parity::*/ ParityMark:
                // Mark parity is not supported by POSIX systems
                m_comConfigure.dcb.fParity = TRUE;
                break;
        }
        SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
    }
}

itas109::Parity CSerialPortWinBase::getParity() const
{
    return m_parity;
}

void CSerialPortWinBase::setDataBits(itas109::DataBits dataBits)
{
    itas109::IAutoLock lock(p_mutex);
    m_dataBits = dataBits;

    if (isOpen())
    {
        switch (dataBits)
        {
            case itas109::/*DataBits::*/ DataBits5: // 5 data bits
                if (m_stopbits == itas109::/*StopBits::*/ StopTwo)
                {
                    // 5 Data bits cannot be used with 2 stop bits
                }
                else
                {
                    m_comConfigure.dcb.ByteSize = 5;
                    SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
                }
                break;
            case itas109::/*DataBits::*/ DataBits6: // 6 data bits
                if (m_stopbits == itas109::/*StopBits::*/ StopOneAndHalf)
                {
                    // 6 Data bits cannot be used with 1.5 stop bits
                }
                else
                {
                    m_comConfigure.dcb.ByteSize = 6;
                    SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
                }
                break;
            case itas109::/*DataBits::*/ DataBits7: // 7 data bits
                if (m_stopbits == itas109::/*StopBits::*/ StopOneAndHalf)
                {
                    // 7 Data bits cannot be used with 1.5 stop bits
                }
                else
                {
                    m_comConfigure.dcb.ByteSize = 7;
                    SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
                }
                break;
            case itas109::/*DataBits::*/ DataBits8: // 8 data bits
                if (m_stopbits == itas109::/*StopBits::*/ StopOneAndHalf)
                {
                    // 8 Data bits cannot be used with 1.5 stop bits
                }
                else
                {
                    m_comConfigure.dcb.ByteSize = 8;
                    SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
                }
                break;
        }
    }
}

itas109::DataBits CSerialPortWinBase::getDataBits() const
{
    return m_dataBits;
}

void CSerialPortWinBase::setStopBits(itas109::StopBits stopbits)
{
    itas109::IAutoLock lock(p_mutex);
    m_stopbits = stopbits;

    if (isOpen())
    {
        switch (m_stopbits)
        {
            case itas109::/*StopBits::*/ StopOne: // 1 stop bit
                m_comConfigure.dcb.StopBits = ONESTOPBIT;
                SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
                break;
            case itas109::/*StopBits::*/ StopOneAndHalf: // 1.5 stop bit - This is only for the Windows platform
                if (m_dataBits == itas109::/*DataBits::*/ DataBits5)
                {
                    //	1.5 stop bits can only be used with 5 data bits
                }
                else
                {
                    m_comConfigure.dcb.StopBits = ONE5STOPBITS;
                    SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
                }
                break;

                /*two stop bits*/
            case itas109::/*StopBits::*/ StopTwo: // 2 stop bit
                if (m_dataBits == itas109::/*DataBits::*/ DataBits5)
                {
                    // 2 stop bits cannot be used with 5 data bits
                }
                else
                {
                    m_comConfigure.dcb.StopBits = TWOSTOPBITS;
                    SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
                }
                break;
        }
    }
}

itas109::StopBits CSerialPortWinBase::getStopBits() const
{
    return m_stopbits;
}

void CSerialPortWinBase::setFlowControl(itas109::FlowControl flowControl)
{
    itas109::IAutoLock lock(p_mutex);

    m_flowControl = flowControl;

    if (isOpen())
    {
        switch (m_flowControl)
        {
            case itas109::/*FlowControl::*/ FlowNone: // No flow control

                m_comConfigure.dcb.fOutxCtsFlow = FALSE;
                m_comConfigure.dcb.fRtsControl = RTS_CONTROL_DISABLE;
                m_comConfigure.dcb.fInX = FALSE;
                m_comConfigure.dcb.fOutX = FALSE;
                SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
                break;

            case itas109::/*FlowControl::*/ FlowSoftware: // Software(XON / XOFF) flow control
                m_comConfigure.dcb.fOutxCtsFlow = FALSE;
                m_comConfigure.dcb.fRtsControl = RTS_CONTROL_DISABLE;
                m_comConfigure.dcb.fInX = TRUE;
                m_comConfigure.dcb.fOutX = TRUE;
                SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
                break;

            case itas109::/*FlowControl::*/ FlowHardware: // Hardware(RTS / CTS) flow control
                m_comConfigure.dcb.fOutxCtsFlow = TRUE;
                m_comConfigure.dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
                m_comConfigure.dcb.fInX = FALSE;
                m_comConfigure.dcb.fOutX = FALSE;
                SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
                break;
        }
    }
}

itas109::FlowControl CSerialPortWinBase::getFlowControl() const
{
    return m_flowControl;
}

void CSerialPortWinBase::setReadBufferSize(unsigned int size)
{
    itas109::IAutoLock lock(p_mutex);
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
    itas109::IAutoLock lock(p_mutex);
    if (isOpen())
    {
        EscapeCommFunction(m_handle, set ? SETDTR : CLRDTR);
    }
}

void CSerialPortWinBase::setRts(bool set /*= true*/)
{
    itas109::IAutoLock lock(p_mutex);
    if (isOpen())
    {
        EscapeCommFunction(m_handle, set ? SETRTS : CLRRTS);
    }
}

OVERLAPPED CSerialPortWinBase::getOverlapMonitor()
{
    // Finished
    return overlapMonitor;
}

HANDLE CSerialPortWinBase::getMainHandle()
{
    // Finished
    return m_handle;
}

bool CSerialPortWinBase::isThreadRunning()
{
    return m_isThreadRunning;
}

bool CSerialPortWinBase::startThreadMonitor()
{
    // Finished

    // start event thread monitor
    bool bRet = false;
    if (0 == itas109::i_thread_create(&m_monitorThread, NULL, commThreadMonitor, (LPVOID)this))
    {
        bRet = true;
    }
    else
    {
        bRet = false;
    }

    return bRet;
}

bool CSerialPortWinBase::stopThreadMonitor()
{
    // Finished

    SetCommMask(m_monitorThread, 0);
    m_isThreadRunning = false;
    //_endthreadex(0);//not recommend

    return true;
}
