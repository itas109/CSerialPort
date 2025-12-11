#include "CSerialPort/SerialPortWinBase.h"
#include "CSerialPort/SerialPortListener.h"
#include "CSerialPort/IProtocolParser.h"
#include "CSerialPort/ibuffer.hpp"
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
    , m_handle(INVALID_HANDLE_VALUE)
    , m_overlapMonitor()
    , m_overlapRead()
    , m_overlapWrite()
    , m_comConfigure()
    , m_comTimeout()
{
    m_overlapMonitor.Internal = 0;
    m_overlapMonitor.InternalHigh = 0;
    m_overlapMonitor.Offset = 0;
    m_overlapMonitor.OffsetHigh = 0;
    m_overlapMonitor.hEvent = CreateEvent(nullptr, true, false, nullptr);
}

CSerialPortWinBase::~CSerialPortWinBase()
{
    CloseHandle(m_overlapMonitor.hEvent);
}

bool CSerialPortWinBase::openPort()
{
    itas109::IScopedLock lock(m_mutex);

    LOG_INFO("portName: %s, baudRate: %d, dataBit: %d, parity: %d, stopBit: %d, flowControl: %d, mode: %s, readBufferSize:%u(%u), readIntervalTimeoutMS: %u, minByteReadNotify: %u, byteReadBufferFullNotify: %u",
             m_portName, m_baudRate, m_dataBits, m_parity, m_stopbits, m_flowControl, m_operateMode == itas109::AsynchronousOperate ? "async" : "sync", m_readBufferSize, p_readBuffer->getBufferSize(), m_readIntervalTimeoutMS, m_minByteReadNotify, m_byteReadBufferFullNotify);

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
                        m_isEnableReadThread = true;
                        bRet = startReadThread();

                        if (bRet)
                        {
                            m_lastError = itas109::/*SerialPortError::*/ ErrorOK;
                        }
                        else
                        {
                            m_isEnableReadThread = false;
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
    LOG_INFO("%s close...", m_portName);

    if (isOpen())
    {
        stopReadThread();

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

    LOG_INFO("%s close success", m_portName);
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
            usedLen = p_readBuffer->getUsedLen();
        }
        else
        {
            usedLen = getReadBufferUsedLenNative();
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
            numBytes = p_readBuffer->read((char *)data, size);
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
        LOG_INFO("%s write. len: %d, hex(top100): %s", m_portName, size, itas109::IUtils::charToHexStr(hexStr, (const char *)data, size > 100 ? 100 : size));
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

void CSerialPortWinBase::beforeStopReadThread()
{
    // 唤醒等待的线程，避免join()阻塞
    if (INVALID_HANDLE_VALUE != m_handle)
    {
        SetCommMask(m_handle, 0);          // stop WaitCommEvent
        SetEvent(m_overlapMonitor.hEvent); // stop WaitForSingleObject
    }
}

int CSerialPortWinBase::readDataNative(void *data, int size)
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

unsigned int CSerialPortWinBase::getReadBufferUsedLenNative()
{
    DWORD dwError = 0;
    COMSTAT comstat;
    ClearCommError(m_handle, &dwError, &comstat);
    return (unsigned int)comstat.cbInQue;
}

bool CSerialPortWinBase::waitCommEventNative()
{
    DWORD eventMask = 0;
    if (!WaitCommEvent(m_handle, &eventMask, &m_overlapMonitor))
    {
        if (ERROR_IO_PENDING == GetLastError())
        {
            WaitForSingleObject(m_overlapMonitor.hEvent, INFINITE);
        }
    }

    return (eventMask & EV_RXCHAR);
}