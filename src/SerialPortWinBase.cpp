#include "SerialPortWinBase.h"

#include <iostream>

sigslot::signal0<> CSerialPortWinBase::readReady;//sigslot
bool CSerialPortWinBase::isThreadRunning = false;

CSerialPortWinBase::CSerialPortWinBase()
{
	construct();
}

CSerialPortWinBase::CSerialPortWinBase(const std::string & portName)
{
	construct();
}

CSerialPortWinBase::~CSerialPortWinBase()
{
	DeleteCriticalSection(&m_communicationMutex);
}

void CSerialPortWinBase::construct()
{
	m_handle = INVALID_HANDLE_VALUE;
	m_monitorThread = INVALID_HANDLE_VALUE;

	m_baudRate = itas109::BaudRate9600;
	m_parity = itas109::ParityNone;
	m_dataBits = itas109::DataBits8;
	m_stopbits = itas109::StopOne;
	m_flowConctrol = itas109::FlowNone;
	m_readBufferSize = 512;

    m_operateMode = itas109::AsynchronousOperate;

	overlapMonitor.Internal = 0;
	overlapMonitor.InternalHigh = 0;
	overlapMonitor.Offset = 0;
	overlapMonitor.OffsetHigh = 0;
	overlapMonitor.hEvent = CreateEvent(NULL, true, false, NULL);

	isThreadRunning = true;

	InitializeCriticalSection(&m_communicationMutex);
}

void CSerialPortWinBase::init(std::string portName, int baudRate /*= itas109::BaudRate::BaudRate9600*/, itas109::Parity parity /*= itas109::Parity::ParityNone*/, itas109::DataBits dataBits /*= itas109::DataBits::DataBits8*/, itas109::StopBits stopbits /*= itas109::StopBits::StopOne*/, itas109::FlowConctrol flowConctrol /*= itas109::FlowConctrol::FlowNone*/, int64 readBufferSize /*= 512*/)
{
	m_portName = "\\\\.\\" + portName;//support COM10 above \\\\.\\COM10
	m_baudRate = baudRate;
	m_parity = parity;
	m_dataBits = dataBits;
	m_stopbits = stopbits;
	m_flowConctrol = flowConctrol;
	m_readBufferSize = readBufferSize;
}

void CSerialPortWinBase::init(int port, int baudRate /*= itas109::BaudRate9600*/, itas109::Parity parity /*= itas109::ParityNone*/, itas109::DataBits dataBits /*= itas109::DataBits8*/, itas109::StopBits stopbits /*= itas109::StopOne*/, itas109::FlowConctrol flowConctrol /*= itas109::FlowNone*/, int64 readBufferSize /*= 512*/)
{
	char sPort[32]; 
	_itoa_s(port, sPort, 10); 
	std::string portName = "\\\\.\\COM";//support COM10 above \\\\.\\COM10
	portName += sPort;

	init(portName, baudRate, parity, dataBits, stopbits, flowConctrol, readBufferSize);
}

bool CSerialPortWinBase::openPort()
{
	lock();

	bool bRet = false;

    TCHAR tcPortName[MAX_PATH];
#ifdef UNICODE
    _stprintf_s(tcPortName, MAX_PATH, _T("%S"), m_portName.c_str());//%S宽字符
#else
    _stprintf_s(tcPortName, MAX_PATH, _T("%s"), m_portName.c_str());//%s单字符
#endif
	unsigned long configSize = sizeof(COMMCONFIG);
	m_comConfigure.dwSize = configSize;

	DWORD dwFlagsAndAttributes = 0;
	if (m_operateMode == itas109::OperateMode::AsynchronousOperate)
	{
		dwFlagsAndAttributes += FILE_FLAG_OVERLAPPED;
	}

	if (!isOpened())
	{
		// get a handle to the port
        m_handle = CreateFile(tcPortName,						// communication port string (COMX)
			GENERIC_READ | GENERIC_WRITE,	// read/write types
			0,								// comm devices must be opened with exclusive access
			NULL,							// no security attributes
			OPEN_EXISTING,					// comm devices must use OPEN_EXISTING
			dwFlagsAndAttributes,			// Async I/O or sync I/O
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
			//m_comConfigure.dcb.fDtrControl;
			//m_comConfigure.dcb.fRtsControl;

			m_comConfigure.dcb.fBinary = true;
			m_comConfigure.dcb.fInX = false;
			m_comConfigure.dcb.fOutX = false;
			m_comConfigure.dcb.fAbortOnError = false;
			m_comConfigure.dcb.fNull = false;


			//setBaudRate(m_baudRate);
			//setDataBits(m_dataBits);
			//setStopBits(m_stopbits);
			//setParity(m_parity);

			setFlowConctrol(m_flowConctrol); // @todo

//            COMMTIMEOUTS m_commTimeouts;
//            //set read timeout
//            m_commTimeouts.ReadIntervalTimeout = MAXWORD;
//            m_commTimeouts.ReadTotalTimeoutMultiplier = 0;
//            m_commTimeouts.ReadTotalTimeoutConstant = 0;
//            //set write timeout
//            m_commTimeouts.WriteTotalTimeoutConstant = 500;
//            m_commTimeouts.WriteTotalTimeoutMultiplier = 100;
//            SetCommTimeouts(m_handle,&m_commTimeouts); // @todo for test

			if (SetCommConfig(m_handle, &m_comConfigure, configSize))
			{
				// @todo 
				//Discards all characters from the output or input buffer of a specified communications resource. It can also terminate pending read or write operations on the resource.
				PurgeComm(m_handle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_RXABORT);

				//init event driven approach
				if (m_operateMode == itas109::OperateMode::AsynchronousOperate)
				{
					m_comTimeout.ReadIntervalTimeout = MAXDWORD;
					m_comTimeout.ReadTotalTimeoutMultiplier = 0;
					m_comTimeout.ReadTotalTimeoutConstant = 0;
					m_comTimeout.WriteTotalTimeoutMultiplier = 0;
					m_comTimeout.WriteTotalTimeoutConstant = 0;
					SetCommTimeouts(m_handle, &m_comTimeout);

					//set comm event
					if (SetCommMask(m_handle, EV_TXEMPTY | EV_RXCHAR | EV_DSR)) // @todo mask need modify
					{
						isThreadRunning = true;
						bRet = startThreadMonitor();

						if (!bRet)
						{
							isThreadRunning = false;
						}
					}
					else
					{
						//Failed to set Comm Mask
						bRet = false;
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
				//set com configure error
			}
		}
		else
		{
			//串口打开失败，增加提示信息
			switch (GetLastError())
			{
				//串口不存在
			case ERROR_FILE_NOT_FOUND:
			{
										 TCHAR Temp[200] = { 0 };
#ifdef UNICODE
										 _stprintf_s(Temp, 200, _T("%S ERROR_FILE_NOT_FOUND,Error Code:%d"), m_portName.c_str(), GetLastError());
#else
										 _stprintf_s(Temp, 200, _T("%s ERROR_FILE_NOT_FOUND,Error Code:%d"), m_portName.c_str(), GetLastError());
#endif // UNICODE
										 MessageBox(NULL, Temp, _T("COM InitPort Error"), MB_ICONERROR);
										 break;
			}
				//串口拒绝访问
			case ERROR_ACCESS_DENIED:
			{
										TCHAR Temp[200] = { 0 };
#ifdef UNICODE
										_stprintf_s(Temp, 200, _T("%S ERROR_ACCESS_DENIED,Error Code:%d"), m_portName.c_str(), GetLastError());
#else
										_stprintf_s(Temp, 200, _T("%s ERROR_ACCESS_DENIED,Error Code:%d"), m_portName.c_str(), GetLastError());
#endif // UNICODE
										MessageBox(NULL, Temp, _T("COM InitPort Error"), MB_ICONERROR);
										break;
			}
			default:
				break;
			}
		}
	}
	else
	{
		bRet = false;
	}

	unlock();

	return bRet;
}

void CSerialPortWinBase::closePort()
{
	//Finished
	if (isOpened())
	{
		stopThreadMonitor();

		//Discards all characters from the output or input buffer of a specified communications resource. It can also terminate pending read or write operations on the resource.
		::PurgeComm(m_handle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_RXABORT);

		::CloseHandle(m_handle);
		m_handle = INVALID_HANDLE_VALUE;
	}
}

unsigned int __stdcall CSerialPortWinBase::commThreadMonitor(LPVOID pParam)
{
	// Cast the void pointer passed to the thread back to
	// a pointer of CSerialPortWinBase class
	CSerialPortWinBase *p_base = (CSerialPortWinBase*)pParam;

	int iRet = 0;

	DWORD dwError = 0;
	COMSTAT comstat;

	if (p_base)
	{
		DWORD eventMask = 0;

		HANDLE m_mainHandle = p_base->getMainHandle();
		OVERLAPPED m_overlapMonitor = p_base->getOverlapMonitor();

		ResetEvent(m_mainHandle);

		while (isThreadRunning)
		{
			if (!WaitCommEvent(m_mainHandle, &eventMask, &m_overlapMonitor))
			if (GetLastError() != ERROR_IO_PENDING)
			{
				//WaitCommEvent error
			}

			if (WaitForSingleObject(m_overlapMonitor.hEvent, INFINITE) == WAIT_OBJECT_0)
			{
				//overlap event occured
				DWORD undefined;
				if (!GetOverlappedResult(m_mainHandle, &m_overlapMonitor, &undefined, false))
				{
					//Comm event overlapped error
					return 1;//@ todo
				}

				if (eventMask & EV_RXCHAR)
				{

					std::cout << "EV_RXCHAR" << std::endl;

					// solve 线程中循环的低效率问题
					ClearCommError(m_mainHandle, &dwError, &comstat);
                    if (comstat.cbInQue >= p_base->m_minByteReadNoify) //设定字符数,默认为2
					{
                        readReady._emit();
					}
				}

				if (eventMask & EV_TXEMPTY)
				{
					DWORD numBytes;
					GetOverlappedResult(m_mainHandle, &m_overlapMonitor, &numBytes, true);
					std::cout << "EV_TXEMPTY" << std::endl;
				}

				if (eventMask & EV_DSR)
				{
					std::cout << "EV_DSR" << std::endl;
				}
			}
		}
	}
	else
	{
		//point null
		iRet = 0;
	}

	return iRet;
}

bool CSerialPortWinBase::isOpened()
{
	//Finished
	return m_handle != INVALID_HANDLE_VALUE;
}

int CSerialPortWinBase::readData(char *data, int maxSize)
{
	DWORD dRet;

	lock();

	if (isOpened())
	{
		if (m_operateMode == itas109::OperateMode::AsynchronousOperate)
		{
			m_overlapRead.Internal = 0;
			m_overlapRead.InternalHigh = 0;
			m_overlapRead.Offset = 0;
			m_overlapRead.OffsetHigh = 0;
			m_overlapRead.hEvent = CreateEvent(NULL, true, false, NULL);
			if (ReadFile(m_handle, (void*)data, (DWORD)maxSize, &dRet, &m_overlapRead))
			{
				data[dRet] = '\0';
			}
			else
			{
				if (GetLastError() == ERROR_IO_PENDING)//Normal, No data read - error code:997
				{
					GetOverlappedResult(m_handle, &m_overlapRead, &dRet, true);
				}
				else
				{
					lastError = itas109::SerialPortError::ReadError;
					dRet = (DWORD)-1;
				}
			}
			CloseHandle(m_overlapRead.hEvent);
		}
		else
		{
			if (ReadFile(m_handle, (void*)data, (DWORD)maxSize, &dRet, NULL))
			{

			}
			else
			{
				lastError = itas109::SerialPortError::ReadError;
				dRet = (DWORD)-1;
			}
		}
	}
	else
	{
		lastError = itas109::SerialPortError::NotOpenError;
		dRet = (DWORD)-1;
	}

	unlock();

	return dRet;
}

int CSerialPortWinBase::readAllData(char *data)
{
	int maxSize = 0;

    if (m_operateMode == itas109::OperateMode::AsynchronousOperate)
    {
        DWORD dwError = 0;
        COMSTAT comstat;
        ClearCommError(m_handle, &dwError, &comstat);
        maxSize = comstat.cbInQue;
    }
    else
    {
        maxSize = 1024;//Synchronous ClearCommError not work
    }

    return readData(data, maxSize);
}

int CSerialPortWinBase::readLineData(char *data, int maxSize)
{
	DWORD dRet;
	lock();

	if (isOpened())
	{
	}
	else
	{
		lastError = itas109::SerialPortError::NotOpenError;
		dRet = (DWORD)-1;
	}

	unlock();

	return dRet;
}

int CSerialPortWinBase::writeData(const char * data, int maxSize)
{
	DWORD dRet;

	lock();

	if (isOpened())
	{
		// @todo maybe mutile thread not need this
		//Discards all characters from the output or input buffer of a specified communications resource. It can also terminate pending read or write operations on the resource.
		//::PurgeComm(m_handle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_RXABORT);

		if (m_operateMode == itas109::OperateMode::AsynchronousOperate)
		{
			m_overlapWrite.Internal = 0;
			m_overlapWrite.InternalHigh = 0;
			m_overlapWrite.Offset = 0;
			m_overlapWrite.OffsetHigh = 0;
			m_overlapWrite.hEvent = CreateEvent(NULL, true, false, NULL);
			if (WriteFile(m_handle, (void*)data, (DWORD)maxSize, &dRet, &m_overlapWrite))
			{
				dRet = maxSize;
			}
			else
			{
				lastError = itas109::SerialPortError::WriteError;
				dRet = (DWORD)-1;
			}
		}
		else
		{
			if (WriteFile(m_handle, (void*)data, (DWORD)maxSize, &dRet, NULL))
			{

			}
			else
			{
				lastError = itas109::SerialPortError::WriteError;;
				dRet = (DWORD)-1;
			}
		}
	}
	else
	{
		lastError = itas109::SerialPortError::NotOpenError;
		dRet = (DWORD)-1;
	}




	unlock();

	return dRet;
}

void CSerialPortWinBase::setDebugModel(bool isDebug)
{
	//@todo
}

void CSerialPortWinBase::setReadTimeInterval(int msecs)
{
    //@todo
}

void CSerialPortWinBase::setMinByteReadNoify(unsigned int minByteReadNoify)
{
    m_minByteReadNoify = minByteReadNoify;
}

int CSerialPortWinBase::getLastError() const
{
	return lastError;
}

void CSerialPortWinBase::clearError()
{
	lastError = itas109::SerialPortError::NoError;
}

void CSerialPortWinBase::setPortName(std::string portName)
{
	// Windows : COM1 
	// Linux : /dev/ttyS0
	m_portName = "\\\\.\\" + portName;//support COM10 above \\\\.\\COM10
}

std::string CSerialPortWinBase::getPortName() const
{
	return m_portName;
}

void CSerialPortWinBase::setBaudRate(int baudRate)
{
	lock();
	m_baudRate = baudRate;
	m_comConfigure.dcb.BaudRate = m_baudRate;
	SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));

	unlock();
}

int CSerialPortWinBase::getBaudRate() const
{
	return m_baudRate;
}

void CSerialPortWinBase::setParity(itas109::Parity parity)
{
	lock();
	m_parity = parity;

	if (isOpened())
	{
		m_comConfigure.dcb.Parity = (unsigned char)parity;
		switch (parity)
		{
		case itas109::Parity::ParityNone:
			m_comConfigure.dcb.fParity = FALSE;
			break;
		case itas109::Parity::ParityOdd:
			m_comConfigure.dcb.fParity = TRUE;
			break;
		case itas109::Parity::ParityEven:
			m_comConfigure.dcb.fParity = TRUE;
			break;
		case itas109::Parity::ParitySpace:
			if (m_dataBits == itas109::DataBits::DataBits8)
			{
				//Space parity with 8 data bits is not supported by POSIX systems
			}
			m_comConfigure.dcb.fParity = TRUE;
			break;
		case itas109::Parity::ParityMark:
			//Mark parity is not supported by POSIX systems
			m_comConfigure.dcb.fParity = TRUE;
			break;
		}
		SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
	}

	unlock();
}

itas109::Parity CSerialPortWinBase::getParity() const
{
	return m_parity;
}

void CSerialPortWinBase::setDataBits(itas109::DataBits dataBits)
{
	lock();
	m_dataBits = dataBits;

	if (isOpened())
	{
		switch (dataBits)
		{
		case itas109::DataBits::DataBits5://5 data bits
			if (m_stopbits == itas109::StopBits::StopTwo)
			{
				//5 Data bits cannot be used with 2 stop bits
			}
			else {
				m_comConfigure.dcb.ByteSize = 5;
				SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
			}
			break;
		case itas109::DataBits::DataBits6://6 data bits
			if (m_stopbits == itas109::StopBits::StopOneAndHalf)
			{
				//6 Data bits cannot be used with 1.5 stop bits
			}
			else
			{
				m_comConfigure.dcb.ByteSize = 6;
				SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
			}
			break;
		case itas109::DataBits::DataBits7://7 data bits
			if (m_stopbits == itas109::StopBits::StopOneAndHalf)
			{
				//7 Data bits cannot be used with 1.5 stop bits
			}
			else
			{
				m_comConfigure.dcb.ByteSize = 7;
				SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
			}
			break;
		case itas109::DataBits::DataBits8://8 data bits
			if (m_stopbits == itas109::StopBits::StopOneAndHalf)
			{
				//8 Data bits cannot be used with 1.5 stop bits
			}
			else
			{
				m_comConfigure.dcb.ByteSize = 8;
				SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
			}
			break;
		}
	}
	unlock();
}

itas109::DataBits CSerialPortWinBase::getDataBits() const
{
	return m_dataBits;
}

void CSerialPortWinBase::setStopBits(itas109::StopBits stopbits)
{
	lock();
	m_stopbits = stopbits;

	if (isOpened())
	{
		switch (m_stopbits)
		{
		case itas109::StopBits::StopOne://1 stop bit
			m_comConfigure.dcb.StopBits = ONESTOPBIT;
			SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
			break;
		case itas109::StopBits::StopOneAndHalf://1.5 stop bit - This is only for the Windows platform
			if (m_dataBits == itas109::DataBits::DataBits5)
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
		case itas109::StopBits::StopTwo://2 stop bit
			if (m_dataBits == itas109::DataBits::DataBits5)
			{
				//2 stop bits cannot be used with 5 data bits
			}
			else
			{
				m_comConfigure.dcb.StopBits = TWOSTOPBITS;
				SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
			}
			break;
		}
	}
	unlock();
}

itas109::StopBits CSerialPortWinBase::getStopBits() const
{
	return m_stopbits;
}

void CSerialPortWinBase::setFlowConctrol(itas109::FlowConctrol flowConctrol)
{
	lock();

	m_flowConctrol = flowConctrol;

	if (isOpened())
	{
		switch (m_flowConctrol)
		{
		case itas109::FlowConctrol::FlowNone://No flow control

			m_comConfigure.dcb.fOutxCtsFlow = FALSE;
			m_comConfigure.dcb.fRtsControl = RTS_CONTROL_DISABLE;
			m_comConfigure.dcb.fInX = FALSE;
			m_comConfigure.dcb.fOutX = FALSE;
			SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
			break;

		case itas109::FlowConctrol::FlowSoftware://Software(XON / XOFF) flow control
			m_comConfigure.dcb.fOutxCtsFlow = FALSE;
			m_comConfigure.dcb.fRtsControl = RTS_CONTROL_DISABLE;
			m_comConfigure.dcb.fInX = TRUE;
			m_comConfigure.dcb.fOutX = TRUE;
			SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
			break;

		case itas109::FlowConctrol::FlowHardware://Hardware(RTS / CTS) flow control
			m_comConfigure.dcb.fOutxCtsFlow = TRUE;
			m_comConfigure.dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
			m_comConfigure.dcb.fInX = FALSE;
			m_comConfigure.dcb.fOutX = FALSE;
			SetCommConfig(m_handle, &m_comConfigure, sizeof(COMMCONFIG));
			break;
		}
	}

	unlock();
}

itas109::FlowConctrol CSerialPortWinBase::getFlowConctrol() const
{
	return m_flowConctrol;
}

void CSerialPortWinBase::setReadBufferSize(int64 size)
{
	lock();
	if (isOpened())
	{
		m_readBufferSize = size;
	}
	unlock();
}

int64 CSerialPortWinBase::getReadBufferSize() const
{
	return m_readBufferSize;
}

void CSerialPortWinBase::setDtr(bool set /*= true*/)
{
	lock();
	if (isOpened())
	{
		if (set)
		{
			EscapeCommFunction(m_handle, SETDTR);
		}
		else
		{
			EscapeCommFunction(m_handle, CLRDTR);
		}
	}
	unlock();
}

void CSerialPortWinBase::setRts(bool set /*= true*/)
{
	lock();
	if (isOpened())
	{
		if (set)
		{
			EscapeCommFunction(m_handle, SETRTS);
		}
		else
		{
			EscapeCommFunction(m_handle, CLRRTS);
		}
	}
	unlock();
}

std::string CSerialPortWinBase::getVersion()
{
	//Finished
	std::string m_version = "CSerialPortWinBase V1.0.0.181117";
	return m_version;
}

OVERLAPPED CSerialPortWinBase::getOverlapMonitor()
{
	//Finished
	return overlapMonitor;
}

HANDLE CSerialPortWinBase::getMainHandle()
{
	//Finished
	return m_handle;
}

void CSerialPortWinBase::lock()
{
	//Finished
	EnterCriticalSection(&m_communicationMutex);
}

void CSerialPortWinBase::unlock()
{
	//Finished
	LeaveCriticalSection(&m_communicationMutex);
}

bool CSerialPortWinBase::startThreadMonitor()
{
	//Finished

	// start event thread monitor
	bool bRet = false;
	m_monitorThread = (HANDLE)_beginthreadex(NULL, 0, commThreadMonitor, (LPVOID)this, 0, NULL);
	//closeHandle(m_monitorThread);
	if (m_monitorThread != INVALID_HANDLE_VALUE)
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
	//Finished

	SetCommMask(m_monitorThread, 0);
	isThreadRunning = false;
	//_endthreadex(0);//not recommend

	return true;
}
