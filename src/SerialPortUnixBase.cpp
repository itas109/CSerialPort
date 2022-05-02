#include "CSerialPort/SerialPortUnixBase.h"
#include "CSerialPort/ithread.hpp"
#include <unistd.h> // usleep

CSerialPortUnixBase::CSerialPortUnixBase()
{
    construct();
}

CSerialPortUnixBase::CSerialPortUnixBase(const std::string &portName)
{
    construct();
}

CSerialPortUnixBase::~CSerialPortUnixBase() {}

void CSerialPortUnixBase::construct()
{
    fd = -1;

    m_baudRate = itas109::BaudRate9600;
    m_parity = itas109::ParityNone;
    m_dataBits = itas109::DataBits8;
    m_stopbits = itas109::StopOne;
    m_flowControl = itas109::FlowNone;
    m_readBufferSize = 512;

    m_isThreadRunning = false;

    m_operateMode = itas109::AsynchronousOperate;
}

void CSerialPortUnixBase::init(std::string portName,
                               int baudRate /*= itas109::BaudRate::BaudRate9600*/,
                               itas109::Parity parity /*= itas109::Parity::ParityNone*/,
                               itas109::DataBits dataBits /*= itas109::DataBits::DataBits8*/,
                               itas109::StopBits stopbits /*= itas109::StopBits::StopOne*/,
                               itas109::FlowControl flowControl /*= itas109::FlowControl::FlowNone*/,
                               unsigned int readBufferSize /*= 512*/)
{
    m_portName = portName; // portName;//串口 /dev/ttySn, USB /dev/ttyUSBn
    m_baudRate = baudRate;
    m_parity = parity;
    m_dataBits = dataBits;
    m_stopbits = stopbits;
    m_flowControl = flowControl;
    m_readBufferSize = readBufferSize;
}

int CSerialPortUnixBase::uart_set(int fd, int baudRate, itas109::Parity parity, itas109::DataBits dataBits, itas109::StopBits stopbits, itas109::FlowControl flowControl)
{
    struct termios options;

    //获取终端属性
    if (tcgetattr(fd, &options) < 0)
    {
        perror("tcgetattr error");
        return -1;
    }

    //设置输入输出波特率
    int baudRateConstant = 0;
    baudRateConstant = rate2Constant(baudRate);

    if (0 != baudRateConstant)
    {
        cfsetispeed(&options, baudRateConstant);
        cfsetospeed(&options, baudRateConstant);
    }
    else
    {
        // TODO: custom baudrate
        fprintf(stderr, "Unkown baudrate!\n");
        return -1;
    }

    //设置校验位
    switch (parity)
    {
        /*无奇偶校验位*/
        case itas109::ParityNone:
        case 'N':
            options.c_cflag &= ~PARENB; // PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~INPCK;  // INPCK：使奇偶校验起作用
            break;
        /*设置奇校验*/
        case itas109::ParityOdd:
            options.c_cflag |= PARENB; // PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag |= PARODD; // PARODD：若设置则为奇校验,否则为偶校验
            options.c_cflag |= INPCK;  // INPCK：使奇偶校验起作用
            options.c_cflag |= ISTRIP; // ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
            break;
        /*设置偶校验*/
        case itas109::ParityEven:
            options.c_cflag |= PARENB;  // PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~PARODD; // PARODD：若设置则为奇校验,否则为偶校验
            options.c_cflag |= INPCK;   // INPCK：使奇偶校验起作用
            options.c_cflag |= ISTRIP;  // ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
            break;
            /*设为空格,即停止位为2位*/
        case itas109::ParitySpace:
            options.c_cflag &= ~PARENB; // PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~CSTOPB; // CSTOPB：使用两位停止位
            break;
        default:
            fprintf(stderr, "Unkown parity!\n");
            return -1;
    }

    //设置数据位
    switch (dataBits)
    {
        case itas109::DataBits5:
            options.c_cflag &= ~CSIZE; //屏蔽其它标志位
            options.c_cflag |= CS5;
            break;
        case itas109::DataBits6:
            options.c_cflag &= ~CSIZE; //屏蔽其它标志位
            options.c_cflag |= CS6;
            break;
        case itas109::DataBits7:
            options.c_cflag &= ~CSIZE; //屏蔽其它标志位
            options.c_cflag |= CS7;
            break;
        case itas109::DataBits8:
            options.c_cflag &= ~CSIZE; //屏蔽其它标志位
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr, "Unkown bits!\n");
            return -1;
    }

    //停止位
    switch (stopbits)
    {
        case itas109::StopOne:
            options.c_cflag &= ~CSTOPB; // CSTOPB：使用两位停止位
            break;
        case itas109::StopOneAndHalf:
            fprintf(stderr, "POSIX does not support 1.5 stop bits!\n");
            return -1;
        case itas109::StopTwo:
            options.c_cflag |= CSTOPB; // CSTOPB：使用两位停止位
            break;
        default:
            fprintf(stderr, "Unkown stop!\n");
            return -1;
    }

    //控制模式
    options.c_cflag |= CLOCAL; //保证程序不占用串口
    options.c_cflag |= CREAD;  //保证程序可以从串口中读取数据

    //流控制
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
            fprintf(stderr, "Unkown c_flow!\n");
            return -1;
    }

    //设置输出模式为原始输出
    options.c_oflag &= ~OPOST; // OPOST：若设置则按定义的输出处理，否则所有c_oflag失效

    //设置本地模式为原始模式
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

    //设置等待时间和最小接受字符
    options.c_cc[VTIME] = 0; //可以在select中设置
    options.c_cc[VMIN] = 1;  //最少读取一个字符

    //如果发生数据溢出，只接受数据，但是不进行读操作
    tcflush(fd, TCIFLUSH);

    //激活配置
    if (tcsetattr(fd, TCSANOW, &options) < 0)
    {
        perror("tcsetattr failed");
        return -1;
    }

    return 0;
}

void *CSerialPortUnixBase::commThreadMonitor(void *pParam)
{
    // Cast the void pointer passed to the thread back to
    // a pointer of CSerialPortWinBase class
    CSerialPortUnixBase *p_base = (CSerialPortUnixBase *)pParam;

    if (p_base)
    {
        for (; p_base->isThreadRunning();)
        {
            int readbytes = 0;

            // read前获取可读的字节数,不区分阻塞和非阻塞
            ioctl(p_base->fd, FIONREAD, &readbytes);
            if (readbytes >= p_base->getMinByteReadNotify()) //设定字符数，默认为2
            {
                p_base->readReady._emit();
            }
            else
            {
                usleep(1); // fix high cpu usage on unix
            }
        }
    }
    else
    {
        // point null
    }

    pthread_exit(NULL);
}

bool CSerialPortUnixBase::startThreadMonitor()
{
    bool bRet = true;

    // start read thread
    if (0 != itas109::i_thread_create(&m_monitorThread, NULL, commThreadMonitor, (void *)this))
    {
        bRet = false;

        printf("Create read thread error.");
    }

    return bRet;
}

bool CSerialPortUnixBase::stopThreadMonitor()
{
    m_isThreadRunning = false;

    itas109::i_thread_join(m_monitorThread);

    return true;
}

bool CSerialPortUnixBase::openPort()
{
    itas109::IAutoLock lock(p_mutex);

    bool bRet = false;

    // fd = open(m_portName.c_str(),O_RDWR | O_NOCTTY);//阻塞

    fd = open(m_portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY); //非阻塞

    if (fd != -1)
    {
        // if(fcntl(fd,F_SETFL,FNDELAY) >= 0)//非阻塞，覆盖前面open的属性
        if (fcntl(fd, F_SETFL, 0) >= 0) // 阻塞，即使前面在open串口设备时设置的是非阻塞的，这里设为阻塞后，以此为准
        {
            // set param
            if (uart_set(fd, m_baudRate, m_parity, m_dataBits, m_stopbits, m_flowControl) == -1)
            {
                fprintf(stderr, "uart set failed!\n");
                // exit(EXIT_FAILURE);

                bRet = false;
                m_lastError = itas109::/*SerialPortError::*/ InvalidParameterError;
            }
            else
            {
                m_isThreadRunning = true;
                bRet = startThreadMonitor();

                if (!bRet)
                {
                    m_isThreadRunning = false;
                    m_lastError = itas109::/*SerialPortError::*/ SystemError;
                }
            }
        }
        else
        {
            bRet = false;
            m_lastError = itas109::/*SerialPortError::*/ SystemError;
        }
    }
    else
    {
        // Could not open the port
        char str[256];
        snprintf(str, sizeof(str), "open port error: Unable to open %s", m_portName.c_str());
        perror(str);

        bRet = false;
        m_lastError = itas109::/*SerialPortError::*/ OpenError;
    }

    if (!bRet)
    {
        closePort();
    }

    return bRet;
}

void CSerialPortUnixBase::closePort()
{
    if (isOpened())
    {
        stopThreadMonitor();

        close(fd);

        fd = -1;
    }
}

bool CSerialPortUnixBase::isOpened()
{
    return fd != -1;
}

int CSerialPortUnixBase::readData(char *data, int size)
{
    itas109::IAutoLock lock(p_mutex);

    int iRet = -1;

    if (isOpened())
    {
        iRet = read(fd, data, size);
    }
    else
    {
        m_lastError = itas109::/*SerialPortError::*/ NotOpenError;
        iRet = -1;
    }

    return iRet;
}

int CSerialPortUnixBase::readAllData(char *data)
{
    int maxSize = 0;

    // read前获取可读的字节数,不区分阻塞和非阻塞
    ioctl(fd, FIONREAD, &maxSize);

    return readData(data, maxSize);
}

int CSerialPortUnixBase::readLineData(char *data, int size)
{
    itas109::IAutoLock lock(p_mutex);

    int iRet = -1;

    if (isOpened())
    {
    }
    else
    {
        m_lastError = itas109::/*SerialPortError::*/ NotOpenError;
        iRet = -1;
    }

    return iRet;
}

int CSerialPortUnixBase::writeData(const char *data, int size)
{
    itas109::IAutoLock lock(p_mutex);

    int iRet = -1;

    if (isOpened())
    {
        // Write N bytes of BUF to FD.  Return the number written, or -1
        iRet = write(fd, data, size);
    }
    else
    {
        m_lastError = itas109::/*SerialPortError::*/ NotOpenError;
        iRet = -1;
    }

    return iRet;
}

void CSerialPortUnixBase::setDebugModel(bool isDebug)
{
    //@todo
}

void CSerialPortUnixBase::setReadTimeInterval(int msecs)
{
    //@todo
}

void CSerialPortUnixBase::setMinByteReadNotify(unsigned int minByteReadNotify)
{
    m_minByteReadNotify = minByteReadNotify;
}

int CSerialPortUnixBase::getLastError() const
{
    return m_lastError;
}

void CSerialPortUnixBase::clearError()
{
    m_lastError = itas109::NoError;
}

void CSerialPortUnixBase::setPortName(std::string portName)
{
    m_portName = portName;
}

std::string CSerialPortUnixBase::getPortName() const
{
    return m_portName;
}

void CSerialPortUnixBase::setBaudRate(int baudRate)
{
    m_baudRate = baudRate;
}

int CSerialPortUnixBase::getBaudRate() const
{
    return m_baudRate;
}

void CSerialPortUnixBase::setParity(itas109::Parity parity)
{
    m_parity = parity;
}

itas109::Parity CSerialPortUnixBase::getParity() const
{
    return m_parity;
}

void CSerialPortUnixBase::setDataBits(itas109::DataBits dataBits)
{
    m_dataBits = dataBits;
}

itas109::DataBits CSerialPortUnixBase::getDataBits() const
{
    return m_dataBits;
}

void CSerialPortUnixBase::setStopBits(itas109::StopBits stopbits)
{
    m_stopbits = stopbits;
}

itas109::StopBits CSerialPortUnixBase::getStopBits() const
{
    return m_stopbits;
}

void CSerialPortUnixBase::setFlowControl(itas109::FlowControl flowControl)
{
    m_flowControl = flowControl;
}

itas109::FlowControl CSerialPortUnixBase::getFlowControl() const
{
    return m_flowControl;
}

void CSerialPortUnixBase::setReadBufferSize(unsigned int size)
{
    m_readBufferSize = size;
}

unsigned int CSerialPortUnixBase::getReadBufferSize() const
{
    return m_readBufferSize;
}

void CSerialPortUnixBase::setDtr(bool set /*= true*/) {}

void CSerialPortUnixBase::setRts(bool set /*= true*/) {}

bool CSerialPortUnixBase::isThreadRunning()
{
    return m_isThreadRunning;
}

int CSerialPortUnixBase::rate2Constant(int baudrate)
{
    // https://jim.sh/ftx/files/linux-custom-baudrate.c

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
