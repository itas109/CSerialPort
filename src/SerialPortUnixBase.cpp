#include "CSerialPort/SerialPortUnixBase.h"
#include "CSerialPort/SerialPortListener.h"
#include "CSerialPort/ibuffer.hpp"
#include "CSerialPort/ithread.hpp"
#include "CSerialPort/ilog.hpp"

#include <sys/select.h> // select
#include <sys/time.h>   // timeval

#ifdef I_OS_LINUX
// termios2 for custom baud rate at least linux kernel 2.6.32 (RHEL 6.0)

#ifndef I_OS_ANDROID
// linux/include/uapi/asm-generic/termbits.h
struct termios2
{
    tcflag_t c_iflag; /* input mode flags */
    tcflag_t c_oflag; /* output mode flags */
    tcflag_t c_cflag; /* control mode flags */
    tcflag_t c_lflag; /* local mode flags */
    cc_t c_line;      /* line discipline */
    cc_t c_cc[19];    /* control characters */
    speed_t c_ispeed; /* input speed */
    speed_t c_ospeed; /* output speed */
};
#endif

#ifndef BOTHER
#define BOTHER 0010000
#endif

// linux/include/uapi/asm-generic/ioctls.h
#ifndef TCGETS2
#define TCGETS2 _IOR('T', 0x2A, struct termios2)
#endif

#ifndef TCSETS2
#define TCSETS2 _IOW('T', 0x2B, struct termios2)
#endif

#endif

#ifdef I_OS_MAC
#include <IOKit/serial/ioss.h> // IOSSIOSPEED
#endif

CSerialPortUnixBase::CSerialPortUnixBase()
    : CSerialPortUnixBase("")
{
}

CSerialPortUnixBase::CSerialPortUnixBase(const char *portName)
    : CSerialPortAsyncBase(portName)
{
    if (-1 == pipe(pipefd))
    {
        perror("pipe");
    }
}

CSerialPortUnixBase::~CSerialPortUnixBase()
{
    if (isOpen())
    {
        closePort();
    }
}

int CSerialPortUnixBase::uartSet(int fd, int baudRate, itas109::Parity parity, itas109::DataBits dataBits, itas109::StopBits stopbits, itas109::FlowControl flowControl)
{
    struct termios options;

    // 获取终端属性
    if (tcgetattr(m_handle, &options) < 0)
    {
        fprintf(stderr, "tcgetattr error");
        return -1;
    }

    // 设置输入输出波特率
    int baudRateConstant = 0;
    baudRateConstant = rate2Constant(baudRate);

    if (0 != baudRateConstant)
    {
        cfsetispeed(&options, baudRateConstant);
        cfsetospeed(&options, baudRateConstant);
    }

    // 设置校验位
    switch (parity)
    {
        // 无奇偶校验位
        case itas109::ParityNone:
            options.c_cflag &= ~PARENB; // PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~INPCK;  // INPCK：使奇偶校验起作用
            break;
        // 设置奇校验
        case itas109::ParityOdd:
            options.c_cflag |= PARENB; // PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag |= PARODD; // PARODD：若设置则为奇校验,否则为偶校验
            options.c_cflag |= INPCK;  // INPCK：使奇偶校验起作用
            options.c_cflag |= ISTRIP; // ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
            break;
        // 设置偶校验
        case itas109::ParityEven:
            options.c_cflag |= PARENB;  // PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~PARODD; // PARODD：若设置则为奇校验,否则为偶校验
            options.c_cflag |= INPCK;   // INPCK：使奇偶校验起作用
            options.c_cflag |= ISTRIP;  // ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
            break;
        // 设置0校验
        case itas109::ParitySpace:
            options.c_cflag &= ~PARENB; // PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~CSTOPB; // CSTOPB：使用两位停止位
            break;
        default:
            fprintf(stderr, "unknown parity\n");
            return -1;
    }

    // 设置数据位
    switch (dataBits)
    {
        case itas109::DataBits5:
            options.c_cflag &= ~CSIZE; // 屏蔽其它标志位
            options.c_cflag |= CS5;
            break;
        case itas109::DataBits6:
            options.c_cflag &= ~CSIZE; // 屏蔽其它标志位
            options.c_cflag |= CS6;
            break;
        case itas109::DataBits7:
            options.c_cflag &= ~CSIZE; // 屏蔽其它标志位
            options.c_cflag |= CS7;
            break;
        case itas109::DataBits8:
            options.c_cflag &= ~CSIZE; // 屏蔽其它标志位
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr, "unknown data bits\n");
            return -1;
    }

    // 停止位
    switch (stopbits)
    {
        case itas109::StopOne:
            options.c_cflag &= ~CSTOPB; // CSTOPB：使用两位停止位
            break;
        case itas109::StopOneAndHalf:
            fprintf(stderr, "POSIX does not support 1.5 stop bits\n");
            return -1;
        case itas109::StopTwo:
            options.c_cflag |= CSTOPB; // CSTOPB：使用两位停止位
            break;
        default:
            fprintf(stderr, "unknown stop\n");
            return -1;
    }

    // 控制模式
    options.c_cflag |= CLOCAL; // 保证程序不占用串口
    options.c_cflag |= CREAD;  // 保证程序可以从串口中读取数据

    // 流控制
    switch (flowControl)
    {
        case itas109::FlowNone: ///< No flow control 无流控制
            options.c_cflag &= ~CRTSCTS;
            break;
        case itas109::FlowHardware: ///< Hardware(RTS / CTS) flow control 硬件流控制
            options.c_cflag |= CRTSCTS;
            break;
        case itas109::FlowSoftware: ///< Software(XON / XOFF) flow control 软件流控制
            options.c_cflag |= IXON | IXOFF | IXANY;
            break;
        default:
            fprintf(stderr, "unknown flow control\n");
            return -1;
    }

    // 设置输出模式为原始输出
    options.c_oflag &= ~OPOST; // OPOST：若设置则按定义的输出处理，否则所有c_oflag失效

    // 设置本地模式为原始模式
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    /*
     *ICANON：允许规范模式进行输入处理
     *ECHO：允许输入字符的本地回显
     *ECHOE：在接收EPASE时执行Backspace,Space,Backspace组合
     *ISIG：允许信号
     */

    options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /*
     *BRKINT：如果设置了IGNBRK，BREAK键输入将被忽略
     *ICRNL：将输入的回车转化成换行（如果IGNCR未设置的情况下）(0x0d => 0x0a)
     *INPCK：允许输入奇偶校验
     *ISTRIP：去除字符的第8个比特
     *IXON：允许输出时对XON/XOFF流进行控制 (0x11 0x13)
     */

    // 设置等待时间和最小接受字符
    options.c_cc[VTIME] = 0; // 可以在select中设置
    options.c_cc[VMIN] = 0;  // read function will undefinite wait when no read data

    // 如果发生数据溢出，只接受数据，但是不进行读操作
    tcflush(m_handle, TCIFLUSH);

    // 激活配置
    if (tcsetattr(m_handle, TCSANOW, &options) < 0)
    {
        perror("tcsetattr failed");
        return -1;
    }

    // set custom baud rate, after tcsetattr
    if (0 == baudRateConstant)
    {
#ifdef I_OS_LINUX
        struct termios2 tio2;

        if (-1 != ioctl(m_handle, TCGETS2, &tio2))
        {
            tio2.c_cflag &= ~CBAUD; // remove current baud rate
            tio2.c_cflag |= BOTHER; // allow custom baud rate using int input

            tio2.c_ispeed = baudRate; // set the input baud rate
            tio2.c_ospeed = baudRate; // set the output baud rate

            if (-1 == ioctl(m_handle, TCSETS2, &tio2))
            {
                fprintf(stderr, "termios2 set custom baudrate error\n");
                return -1;
            }
        }
        else
        {
            fprintf(stderr, "termios2 ioctl error\n");
            return -1;
        }
#elif defined I_OS_MAC
        // Mac OS X Tiger(10.4.11) support non-standard baud rate through IOSSIOSPEED
        speed_t customBaudRate = (speed_t)baudRate;
        if (-1 == ioctl(m_handle, IOSSIOSPEED, &customBaudRate))
        {
            fprintf(stderr, "ioctl IOSSIOSPEED custom baud rate error\n");
            return -1;
        }
#else
        fprintf(stderr, "not support custom baudrate\n");
        return -1;
#endif
    }

    return 0;
}

bool CSerialPortUnixBase::openPort()
{
    itas109::IScopedLock lock(m_mutex);

    LOG_INFO("portName: %s, baudRate: %d, dataBit: %d, parity: %d, stopBit: %d, flowControl: %d, mode: %s, readBufferSize:%u(%u), readIntervalTimeoutMS: %u, minByteReadNotify: %u, byteReadBufferFullNotify: %u",
             m_portName, m_baudRate, m_dataBits, m_parity, m_stopbits, m_flowControl, m_operateMode == itas109::AsynchronousOperate ? "async" : "sync", m_readBufferSize, p_readBuffer->getBufferSize(), m_readIntervalTimeoutMS, m_minByteReadNotify, m_byteReadBufferFullNotify);

    bool bRet = false;

    m_handle = open(m_portName, O_RDWR | O_NOCTTY); // block for select
    if (INVALID_FILE_HANDLE != m_handle)
    {
        // set param
        if (uartSet(m_handle, m_baudRate, m_parity, m_dataBits, m_stopbits, m_flowControl) == -1)
        {
            fprintf(stderr, "uart set failed\n");

            bRet = false;
            m_lastError = itas109::/*SerialPortError::*/ ErrorInvalidParam;
        }
        else
        {
            if (m_operateMode == itas109::/*OperateMode::*/ AsynchronousOperate)
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
                bRet = true;
                m_lastError = itas109::/*SerialPortError::*/ ErrorOK;
            }
        }
    }
    else
    {
        // Could not open the port
        // char str[300];
        // snprintf(str, sizeof(str), "open port error: Unable to open %s", m_portName);
        // perror(str);

        if (EACCES == errno)
        {
            m_lastError = itas109::/*SerialPortError::*/ ErrorAccessDenied;
        }
        else if (ENOENT == errno)
        {
            m_lastError = itas109::/*SerialPortError::*/ ErrorNotExist;
        }
        else
        {
            m_lastError = itas109::/*SerialPortError::*/ ErrorOpenFailed;
        }

        bRet = false;
    }

    if (!bRet)
    {
        closePort();
    }

    LOG_INFO("open %s. code: %d, message: %s", m_portName, getLastError(), getLastErrorMsg());

    return bRet;
}

void CSerialPortUnixBase::closePort()
{
    LOG_INFO("%s close...", m_portName);

    if (isOpen())
    {
        stopReadThread();

        close(m_handle);

        m_handle = INVALID_FILE_HANDLE;
    }

    LOG_INFO("%s close success", m_portName);
}

unsigned int CSerialPortUnixBase::getReadBufferUsedLen()
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

int CSerialPortUnixBase::readData(void *data, int size)
{
    itas109::IScopedLock lock(m_mutexRead);

    if (size <= 0)
    {
        return 0;
    }

    int iRet = -1;

    if (isOpen())
    {
        if (m_operateMode == itas109::/*OperateMode::*/ AsynchronousOperate)
        {
            iRet = p_readBuffer->read((char *)data, size);
        }
        else
        {
            iRet = read(m_handle, data, size);
        }
    }
    else
    {
        m_lastError = itas109::/*SerialPortError::*/ ErrorNotOpen;
        iRet = -1;
    }

#ifdef CSERIALPORT_DEBUG
    char hexStr[201]; // 100*2 + 1
    LOG_INFO("read. len: %d, hex(top100): %s", iRet, itas109::IUtils::charToHexStr(hexStr, (const char *)data, iRet > 100 ? 100 : iRet));
#endif

    return iRet;
}

int CSerialPortUnixBase::writeData(const void *data, int size)
{
    itas109::IScopedLock lock(m_mutexWrite);

    int iRet = -1;

    if (isOpen())
    {
        // Write N bytes of BUF to FD.  Return the number written, or -1
        iRet = write(m_handle, data, size);
    }
    else
    {
        m_lastError = itas109::/*SerialPortError::*/ ErrorNotOpen;
        iRet = -1;
    }

#ifdef CSERIALPORT_DEBUG
    char hexStr[201]; // 100*2 + 1
    LOG_INFO("%s write. len: %d, hex(top100): %s", m_portName, size, itas109::IUtils::charToHexStr(hexStr, (const char *)data, size > 100 ? 100 : size));
#endif

    return iRet;
}

bool CSerialPortUnixBase::flushBuffers()
{
    itas109::IScopedLock lock(m_mutex);

    if (isOpen())
    {
        return 0 == tcdrain(m_handle);
    }
    else
    {
        return false;
    }
}

bool CSerialPortUnixBase::flushReadBuffers()
{
    itas109::IScopedLock lock(m_mutex);

    if (isOpen())
    {
        return 0 == tcflush(m_handle, TCIFLUSH);
    }
    else
    {
        return false;
    }
}

bool CSerialPortUnixBase::flushWriteBuffers()
{
    itas109::IScopedLock lock(m_mutex);

    if (isOpen())
    {
        return 0 == tcflush(m_handle, TCOFLUSH);
    }
    else
    {
        return false;
    }
}

void CSerialPortUnixBase::setDtr(bool set /*= true*/)
{
    itas109::IScopedLock lock(m_mutex);
    if (isOpen())
    {
        int status = TIOCM_DTR;
        if (ioctl(m_handle, set ? TIOCMBIS : TIOCMBIC, &status) < 0)
        {
            perror("setDtr error");
        }
    }
}

void CSerialPortUnixBase::setRts(bool set /*= true*/)
{
    itas109::IScopedLock lock(m_mutex);
    if (isOpen())
    {
        int status = TIOCM_RTS;
        if (ioctl(m_handle, set ? TIOCMBIS : TIOCMBIC, &status) < 0)
        {
            perror("setRts error");
        }
    }
}

int CSerialPortUnixBase::rate2Constant(int baudrate)
{
#define B(x) \
    case x:  \
        return B##x

    switch (baudrate)
    {
#ifdef B50
        B(50);
#endif
#ifdef B75
        B(75);
#endif
#ifdef B110
        B(110);
#endif
#ifdef B134
        B(134);
#endif
#ifdef B150
        B(150);
#endif
#ifdef B200
        B(200);
#endif
#ifdef B300
        B(300);
#endif
#ifdef B600
        B(600);
#endif
#ifdef B1200
        B(1200);
#endif
#ifdef B1800
        B(1800);
#endif
#ifdef B2400
        B(2400);
#endif
#ifdef B4800
        B(4800);
#endif
#ifdef B9600
        B(9600);
#endif
#ifdef B19200
        B(19200);
#endif
#ifdef B38400
        B(38400);
#endif
#ifdef B57600
        B(57600);
#endif
#ifdef B115200
        B(115200);
#endif
#ifdef B230400
        B(230400);
#endif
#ifdef B460800
        B(460800);
#endif
#ifdef B500000
        B(500000);
#endif
#ifdef B576000
        B(576000);
#endif
#ifdef B921600
        B(921600);
#endif
#ifdef B1000000
        B(1000000);
#endif
#ifdef B1152000
        B(1152000);
#endif
#ifdef B1500000
        B(1500000);
#endif
#ifdef B2000000
        B(2000000);
#endif
#ifdef B2500000
        B(2500000);
#endif
#ifdef B3000000
        B(3000000);
#endif
#ifdef B3500000
        B(3500000);
#endif
#ifdef B4000000
        B(4000000);
#endif
        default:
            return 0;
    }

#undef B
}

void CSerialPortUnixBase::beforeStopReadThread()
{
    if (INVALID_FILE_HANDLE != m_handle)
    {
        write(pipefd[1], "q", 1);
    }
}

int CSerialPortUnixBase::readDataNative(void *data, int size)
{
    itas109::IScopedLock lock(m_mutexRead);

    int iRet = -1;

    if (isOpen())
    {
        iRet = read(m_handle, data, size);
    }
    else
    {
        m_lastError = itas109::/*SerialPortError::*/ ErrorNotOpen;
        iRet = -1;
    }

    return iRet;
}

unsigned int CSerialPortUnixBase::getReadBufferUsedLenNative()
{
    // read前获取可读的字节数,不区分阻塞和非阻塞
    unsigned int readBufferBytes = 0;
    ioctl(m_handle, FIONREAD, &readBufferBytes);
    return readBufferBytes;
}

int CSerialPortUnixBase::waitCommEventNative()
{
    fd_set readFd; // read fdset
    struct timeval timeout;
    timeout.tv_sec = m_readIntervalTimeoutMS / 1000;
    timeout.tv_usec = (m_readIntervalTimeoutMS % 1000) * 1000;

    FD_ZERO(&readFd);           // clear all read fdset
    FD_SET(m_handle, &readFd);  // add read fdset
    FD_SET(pipefd[0], &readFd); // add pipe read fdset for wake up
    int ret = select(m_handle > pipefd[0] ? m_handle + 1 : pipefd[0] + 1, &readFd, nullptr, nullptr, 0 == m_readIntervalTimeoutMS ? NULL : &timeout);
    if (ret < 0) // -1 error, 0 timeout, >0 ok
    {
        if (errno == EINTR) // ignore system interupt
        {
            return -1; // continue
        }
        perror("select error");
        return -1; // break
    }
    else if (0 == ret) // timeout
    {
        return 0; // continue
    }

    if (0 == FD_ISSET(m_handle, &readFd))
    {
        return -1; // continue
    }
    else if (1 == FD_ISSET(pipefd[0], &readFd))
    {
        char buf[1];                       // q
        read(pipefd[0], buf, sizeof(buf)); // clear pipe
        return -1;                         // continue
    }

    return 1; // ok
}