#include "SerialPortUnixBase.h"

sigslot::signal0<> CSerialPortUnixBase::readReady;//sigslot
bool CSerialPortUnixBase::isThreadRunning = false;

CSerialPortUnixBase::CSerialPortUnixBase()
{
    construct();
}

CSerialPortUnixBase::CSerialPortUnixBase(const std::string & portName)
{
    construct();
}

CSerialPortUnixBase::~CSerialPortUnixBase()
{
    pthread_mutex_destroy(&m_communicationMutex);
}

void CSerialPortUnixBase::construct()
{
    fd = -1;

    m_baudRate = itas109::BaudRate9600;
    m_parity = itas109::ParityNone;
    m_dataBits = itas109::DataBits8;
    m_stopbits = itas109::StopOne;
    m_flowConctrol = itas109::FlowNone;
    m_readBufferSize = 512;

    m_operateMode = itas109::AsynchronousOperate;

    pthread_mutex_init(&m_communicationMutex,NULL);
}

void CSerialPortUnixBase::init(std::string portName, int baudRate /*= itas109::BaudRate::BaudRate9600*/, itas109::Parity parity /*= itas109::Parity::ParityNone*/, itas109::DataBits dataBits /*= itas109::DataBits::DataBits8*/, itas109::StopBits stopbits /*= itas109::StopBits::StopOne*/, itas109::FlowConctrol flowConctrol /*= itas109::FlowConctrol::FlowNone*/, int64 readBufferSize /*= 512*/)
{
    m_portName = portName;//portName;//串口 /dev/ttySn, USB /dev/ttyUSBn
    m_baudRate = baudRate;
    m_parity = parity;
    m_dataBits = dataBits;
    m_stopbits = stopbits;
    m_flowConctrol = flowConctrol;
    m_readBufferSize = readBufferSize;
}

void CSerialPortUnixBase::init(int port, int baudRate, itas109::Parity parity, itas109::DataBits dataBits, itas109::StopBits stopbits, itas109::FlowConctrol flowConctrol, int64 readBufferSize)
{
    char sPort[32];
    //_itoa_s(port, sPort, 10);
    std::string portName = "/dev/ttyS";//unix not recommend use port init
    portName += sPort;

    init(portName, baudRate, parity, dataBits, stopbits, flowConctrol, readBufferSize);

}

int CSerialPortUnixBase::uart_set(int fd, int baudRate, itas109::Parity parity, itas109::DataBits dataBits, itas109::StopBits stopbits, itas109::FlowConctrol flowConctrol)
{
    struct termios options;

    //获取终端属性
    if(tcgetattr(fd,&options) < 0)
    {
        perror("tcgetattr error");
        return -1;
    }


    //设置输入输出波特率
    switch(baudRate)
    {
        case itas109::BaudRate1200:
            cfsetispeed(&options,B1200);
            cfsetospeed(&options,B1200);
            break;
        case itas109::BaudRate2400:
            cfsetispeed(&options,B2400);
            cfsetospeed(&options,B2400);
            break;
        case itas109::BaudRate4800:
            cfsetispeed(&options,B4800);
            cfsetospeed(&options,B4800);
            break;
            break;
        case itas109::BaudRate9600:
            cfsetispeed(&options,B9600);
            cfsetospeed(&options,B9600);
            break;
        case itas109::BaudRate19200:
            cfsetispeed(&options,B19200);
            cfsetospeed(&options,B19200);
            break;
        case itas109::BaudRate38400:
            cfsetispeed(&options,B38400);
            cfsetospeed(&options,B38400);
            break;
        default:
            fprintf(stderr,"Unkown baude!\n");
            return -1;
    }

    //设置校验位
    switch(parity)
    {
        /*无奇偶校验位*/
        case itas109::ParityNone:
        case 'N':
            options.c_cflag &= ~PARENB;//PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~INPCK;//INPCK：使奇偶校验起作用
            break;
        /*设置奇校验*/
        case itas109::ParityOdd:
            options.c_cflag |= PARENB;//PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag |= PARODD;//PARODD：若设置则为奇校验,否则为偶校验
            options.c_cflag |= INPCK;//INPCK：使奇偶校验起作用
            options.c_cflag |= ISTRIP;//ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
            break;
        /*设置偶校验*/
        case itas109::ParityEven:
            options.c_cflag |= PARENB;//PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~PARODD;//PARODD：若设置则为奇校验,否则为偶校验
            options.c_cflag |= INPCK;//INPCK：使奇偶校验起作用
            options.c_cflag |= ISTRIP;//ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
            break;
            /*设为空格,即停止位为2位*/
        case itas109::ParitySpace:
            options.c_cflag &= ~PARENB;//PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~CSTOPB;//CSTOPB：使用两位停止位
            break;
        default:
                fprintf(stderr,"Unkown parity!\n");
                return -1;
    }

    //设置数据位
    switch(dataBits)
    {
        case itas109::DataBits5:
            options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS5;
            break;
        case itas109::DataBits6:
            options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS6;
            break;
        case itas109::DataBits7:
            options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS7;
            break;
        case itas109::DataBits8:
            options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr,"Unkown bits!\n");
            return -1;
    }

    //停止位
    switch(stopbits)
    {
        case itas109::StopOne:
            options.c_cflag &= ~CSTOPB;//CSTOPB：使用两位停止位
            break;
        case itas109::StopOneAndHalf:
            fprintf(stderr,"POSIX does not support 1.5 stop bits!\n");
            return -1;
        case itas109::StopTwo:
            options.c_cflag |= CSTOPB;//CSTOPB：使用两位停止位
            break;
        default:
            fprintf(stderr,"Unkown stop!\n");
            return -1;
    }

    //控制模式
    options.c_cflag |= CLOCAL;//保证程序不占用串口
    options.c_cflag |= CREAD;//保证程序可以从串口中读取数据

    //流控制
    switch(flowConctrol)
    {
        case itas109::FlowNone:///< No flow control 无流控制
            options.c_cflag &= ~CRTSCTS;
            break;
        case itas109::FlowHardware:///< Hardware(RTS / CTS) flow control 硬件流控制
            options.c_cflag |= CRTSCTS;
            break;
        case itas109::FlowSoftware:///< Software(XON / XOFF) flow control 软件流控制
            options.c_cflag |= IXON|IXOFF|IXANY;
            break;
        default:
            fprintf(stderr,"Unkown c_flow!\n");
            return -1;
    }

    //设置输出模式为原始输出
    options.c_oflag &= ~OPOST;//OPOST：若设置则按定义的输出处理，否则所有c_oflag失效

    //设置本地模式为原始模式
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    /*
     *ICANON：允许规范模式进行输入处理
     *ECHO：允许输入字符的本地回显
     *ECHOE：在接收EPASE时执行Backspace,Space,Backspace组合
     *ISIG：允许信号
     */

    //设置等待时间和最小接受字符
    options.c_cc[VTIME] = 0;//可以在select中设置
    options.c_cc[VMIN] = 1;//最少读取一个字符

    //如果发生数据溢出，只接受数据，但是不进行读操作
    tcflush(fd,TCIFLUSH);

    //激活配置
    if(tcsetattr(fd,TCSANOW,&options) < 0)
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
    CSerialPortUnixBase *p_base = (CSerialPortUnixBase*)pParam;

    if(p_base)
    {
        while (isThreadRunning)
        {
            int readbytes = 0;

            //read前获取可读的字节数,不区分阻塞和非阻塞
            ioctl(p_base->fd, FIONREAD, &readbytes);
            if (readbytes >= p_base->m_minByteReadNoify) //设定字符数，默认为2
            {
                readReady._emit();
            }
        }
    }
    else
    {
        //point null
    }

    pthread_exit(NULL);
}

bool CSerialPortUnixBase::startThreadMonitor()
{
    bool bRet = true;

    //start read thread
    int ret = pthread_create(&m_monitorThread, NULL, commThreadMonitor, (void*)this);
    if (ret < 0)
    {
        bRet = false;

        printf("Create read thread error.");
    }

    return bRet;
}

bool CSerialPortUnixBase::stopThreadMonitor()
{
    isThreadRunning = false;

    pthread_join( m_monitorThread, NULL );

    return true;
}

bool CSerialPortUnixBase::openPort()
{
    bool bRet = false;

    lock();

    //fd = open(m_portName.c_str(),O_RDWR | O_NOCTTY);//阻塞

    fd = open(m_portName.c_str(),O_RDWR | O_NOCTTY | O_NDELAY);//非阻塞

    if(fd != -1)
    {
        //if(fcntl(fd,F_SETFL,FNDELAY) >= 0)//非阻塞，覆盖前面open的属性
        if(fcntl(fd, F_SETFL, 0) >= 0)// 阻塞，即使前面在open串口设备时设置的是非阻塞的，这里设为阻塞后，以此为准
        {
            //set param
            if(uart_set(fd,m_baudRate,m_parity,m_dataBits,m_stopbits,m_flowConctrol) == -1)
            {
                fprintf(stderr,"uart set failed!\n");
                //exit(EXIT_FAILURE);

                bRet = false;
            }
            else
            {
                isThreadRunning = true;
                bRet = startThreadMonitor();

                if (!bRet)
                {
                    isThreadRunning = false;
                }
            }
        }
        else
        {
            bRet = false;
        }

    }
    else
    {
        //Could not open the port
        char str[256];
        snprintf(str, sizeof(str),"open port error: Unable to open %s", m_portName.c_str());
        perror(str);

        bRet = false;
    }

    unlock();

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

int CSerialPortUnixBase::readData(char *data, int maxSize)
{
    int iRet = -1;
    lock();

    iRet = read(fd,data,maxSize);

    unlock();

    return iRet;
}

int CSerialPortUnixBase::readAllData(char *data)
{
    int readbytes = 0;

    //read前获取可读的字节数,不区分阻塞和非阻塞
    ioctl(fd, FIONREAD, &readbytes);

    return readData(data, readbytes);
}

int CSerialPortUnixBase::readLineData(char *data, int maxSize)
{
    int iRet = -1;
    lock();

    unlock();

    return iRet;
}

int CSerialPortUnixBase::writeData(const char * data, int maxSize)
{
    int iRet = -1;
    lock();

    //Write N bytes of BUF to FD.  Return the number written, or -1
    iRet = write(fd, data, maxSize);

    unlock();

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

void CSerialPortUnixBase::setMinByteReadNoify(unsigned int minByteReadNoify)
{
    m_minByteReadNoify = minByteReadNoify;
}

int CSerialPortUnixBase::getLastError() const
{
    return lastError;
}

void CSerialPortUnixBase::clearError()
{
    lastError = itas109::NoError;
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

void CSerialPortUnixBase::setFlowConctrol(itas109::FlowConctrol flowConctrol)
{
    m_flowConctrol = flowConctrol;
}

itas109::FlowConctrol CSerialPortUnixBase::getFlowConctrol() const
{
    return m_flowConctrol;
}

void CSerialPortUnixBase::setReadBufferSize(int64 size)
{
    m_readBufferSize = size;
}

int64 CSerialPortUnixBase::getReadBufferSize() const
{
    return m_readBufferSize;
}

void CSerialPortUnixBase::setDtr(bool set /*= true*/)
{

}

void CSerialPortUnixBase::setRts(bool set /*= true*/)
{

}

std::string CSerialPortUnixBase::getVersion()
{
    std::string m_version = "CSerialPortUnixBase V1.0.1.190728";
    return m_version;
}

void CSerialPortUnixBase::lock()
{
    pthread_mutex_lock(&m_communicationMutex);
}

void CSerialPortUnixBase::unlock()
{
    pthread_mutex_unlock(&m_communicationMutex);
}
