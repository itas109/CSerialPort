/*
**	FILENAME			CSerialPort.h
**
**	PURPOSE				This class can read, write and watch one serial port.
**						It sends messages to its owner when something happends on the port
**						The class creates a thread for reading and writing so the main
**						program is not blocked.
**
**	CREATION DATE		15-09-1997
**	LAST MODIFICATION	12-11-1997
**
**	AUTHOR				Remon Spekreijse
**
**
************************************************************************************
**  author: mrlong date:2007-12-25
**
**  改进
**	1) 增加 ClosePort
**	2) 增加 WriteToPort 两个方法
**	3) 增加 SendData 与 RecvData 方法
**************************************************************************************
***************************************************************************************
**  author：liquanhai date:2011-11-06
**
**  改进
**	1) 增加 ClosePort 中交出控制权，防止死锁问题
**	2) 增加 ReceiveChar 中防止线程死锁
**************************************************************************************
***************************************************************************************
**  author：viruscamp date:2013-12-04
**
**  改进
**	1) 增加 IsOpen 判断是否打开
**	2) 修正 InitPort 中 parity Odd Even 参数取值错误
**	3) 修改 InitPort 中 portnr 取值范围，portnr>9 时特殊处理
**	4) 取消对 MFC 的依赖，使用 HWND 替代 CWnd，使用 win32 thread 函数而不是 MFC 的
**	5) 增加用户消息编号自定义，方法来自 CnComm
*************************************************************************************** 
***************************************************************************************
**  author: itas109  date:2014-01-10
**  Blog：blog.csdn.net/itas109
**
**  改进
**    1) 解决COM10以上端口无法显示的问题
**    2) 扩展可选择端口，最大值MaxSerialPortNum可以自定义
**    3) 添加QueryKey()和Hkey2ComboBox两个方法，用于自动查询当前有效的串口号。
***************************************************************************************
**  author: itas109  date:2016-05-06
**  Blog：blog.csdn.net/itas109
**
**  改进
**    1) 修复每次打开串口发送一次，当串口无应答时，需要关闭再打开或者接收完数据才能发送的问题。
**		 解决办法：在m_hEventArray中调整m_hWriteEvent的优先级高于读的优先级。CommThread(LPVOID pParam)函数中读写的位置也调换。
**		 参考：http://zhidao.baidu.com/link?url=RSrbPcfTZRULFFd2ziHZPBwnoXv1iCSu_Nmycb_yEw1mklT8gkoNZAkWpl3UDhk8L35DtRPo5VV5kEGpOx-Gea
**    2) 修复停止位在头文件中定义成1导致SetCommState报错的问题，应为1对应的停止位是1.5。UINT stopsbits = ONESTOPBIT
**    3) switch(stopbits)和switch(parity)增加默认情况，增强程序健壮性
** ***************************************************************************************
**  author: itas109  date:2016-06-22
**  Blog：blog.csdn.net/itas109
**
**  改进
**  1） 增加ReceiveStr方法，用于接收字符串（接收缓冲区有多少字符就接收多少字符）。
**      解决ReceiveChar只能接收单个字符的问题。
** ***************************************************************************************
**  author: itas109  date:2016-06-29
**  Blog：blog.csdn.net/itas109
**
**  改进
**  1） 解决RestartMonitoring方法和StopMonitoring方法命令不准确引起的歧义，根据实际作用。
**		将RestartMonitoring更改为ResumeMonitoring，将StopMonitoring更改为SuspendMonitoring。
**	2） 增加IsThreadSuspend方法，用于判断线程是否挂起。
**	3） 改进ClosePort方法，增加线程挂起判断，解决由于线程挂起导致串口关闭死锁的问题。
**  4） 增加IsReceiveString宏定义，用于接收时采用单字节接收还是多字节接收
** ***************************************************************************************
**  author: itas109  date:2016-08-02
**  Blog：blog.csdn.net/itas109
**  改进
**  1） 改进IsOpen方法，m_hComm增加INVALID_HANDLE_VALUE的情况，因为CreateFile方法失败返回的是INVALID_HANDLE_VALUE，不是NULL
**  2） 改进ClosePort方法：增加串口句柄无效的判断(防止关闭死锁)；m_hWriteEvent不使用CloseHandle关闭
**  3） 改进CommThread、ReceiveChar、ReceiveStr和WriteChar方法中异常处理的判断，增加三种判断：串口打开失败(error code:ERROR_INVALID_HANDLE)、连接过程中非法断开(error code:ERROR_BAD_COMMAND)和拒绝访问(error code:ERROR_ACCESS_DENIED)
**  4） 采用安全函数sprintf_s和strcpy_s函数替换掉sprintf和strcpy
**  5） 改进QueryKey方法，用于查询注册表的可用串口值，可以搜索到任意的可用串口
**  6） 改进InitPort方法，串口打开失败，增加提示信息:串口不存在(error code:ERROR_FILE_NOT_FOUND)和串口拒绝访问(error code:ERROR_ACCESS_DENIED)
**  7） 加入viruscamp 取消对 MFC 的依赖
**  8） 改进InitPort方法，如果上次串口是打开，再次调用InitPort方法，关闭串口需要做一定的延时，否则有几率导致ERROR_ACCESS_DENIED拒绝访问，也就是串口占用问题
**  9） 初始化默认波特率修改为9600
**  10）修复一些释放的BUG
**  11）规范了一些错误信息，参考winerror.h --  error code definitions for the Win32 API functions
** ***************************************************************************************
**  author: itas109  date:2016-08-10
**  Blog：blog.csdn.net/itas109
**  改进
**  1） 改进ReceiveStr方法，comstat.cbInQue = 0xcccccccc的情况（如串口异常断开），会导致RXBuff初始化失败
*/

#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__

#ifndef WM_COMM_MSG_BASE 
	#define WM_COMM_MSG_BASE		WM_USER + 617		//!< 消息编号的基点  
#endif

#define WM_COMM_BREAK_DETECTED		WM_COMM_MSG_BASE + 1	// A break was detected on input.
#define WM_COMM_CTS_DETECTED		WM_COMM_MSG_BASE + 2	// The CTS (clear-to-send) signal changed state. 
#define WM_COMM_DSR_DETECTED		WM_COMM_MSG_BASE + 3	// The DSR (data-set-ready) signal changed state. 
#define WM_COMM_ERR_DETECTED		WM_COMM_MSG_BASE + 4	// A line-status error occurred. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY. 
#define WM_COMM_RING_DETECTED		WM_COMM_MSG_BASE + 5	// A ring indicator was detected. 
#define WM_COMM_RLSD_DETECTED		WM_COMM_MSG_BASE + 6	// The RLSD (receive-line-signal-detect) signal changed state. 
#define WM_COMM_RXCHAR				WM_COMM_MSG_BASE + 7	// A character was received and placed in the input buffer. 
#define WM_COMM_RXFLAG_DETECTED		WM_COMM_MSG_BASE + 8	// The event character was received and placed in the input buffer.  
#define WM_COMM_TXEMPTY_DETECTED	WM_COMM_MSG_BASE + 9	// The last character in the output buffer was sent.  
#define WM_COMM_RXSTR               WM_COMM_MSG_BASE + 10   // Receive string

#define MaxSerialPortNum 200   ///有效的串口总个数，不是串口的号 //add by itas109 2014-01-09
#define IsReceiveString  1     //采用何种方式接收：ReceiveString 1多字符串接收（对应响应函数为WM_COMM_RXSTR），ReceiveString 0一个字符一个字符接收（对应响应函数为WM_COMM_RXCHAR）
class CSerialPort
{				 
public:
	// contruction and destruction
	CSerialPort();
	virtual		~CSerialPort(   );

	// port initialisation		
	// UINT stopsbits = ONESTOPBIT   stop is index 0 = 1 1=1.5 2=2 
	// 切记：stopsbits = 1，不是停止位为1。
	// by itas109 20160506
	BOOL		InitPort(HWND pPortOwner, UINT portnr = 1, UINT baud = 9600, 
				char parity = 'N', UINT databits = 8, UINT stopsbits = ONESTOPBIT, 
				DWORD dwCommEvents = EV_RXCHAR | EV_CTS, UINT nBufferSize = 512,
			
				DWORD ReadIntervalTimeout = 1000,
				DWORD ReadTotalTimeoutMultiplier = 1000,
				DWORD ReadTotalTimeoutConstant = 1000,
				DWORD WriteTotalTimeoutMultiplier = 1000,
				DWORD WriteTotalTimeoutConstant = 1000);

	// start/stop comm watching
	///控制串口监视线程
	BOOL		 StartMonitoring();//开始监听
	BOOL		 ResumeMonitoring();//恢复监听
	BOOL		 SuspendMonitoring();//挂起监听
	BOOL         IsThreadSuspend(HANDLE hThread);//判断线程是否挂起 //add by itas109 2016-06-29

	DWORD		 GetWriteBufferSize();///获取写缓冲大小
	DWORD		 GetCommEvents();///获取事件
	DCB			 GetDCB();///获取DCB

///写数据到串口
	void		WriteToPort(char* string);
	void		WriteToPort(char* string,int n); // add by mrlong 2007-12-25
	void		WriteToPort(LPCTSTR string);	 // add by mrlong 2007-12-25
    void		WriteToPort(LPCTSTR string,int n);//add by mrlong 2007-12-2
	void		WriteToPort(BYTE* Buffer, int n);// add by mrlong
	void		ClosePort();					 // add by mrlong 2007-12-2  
	BOOL		IsOpen();

	//void SendData(LPCTSTR lpszData, const int nLength);   //串口发送函数 by mrlong 2008-2-15
	//BOOL RecvData(LPTSTR lpszData, const int nSize);	  //串口接收函数 by mrlong 2008-2-15
	void QueryKey(HKEY hKey);///查询注册表的串口号，将值存于数组中
	void Hkey2ComboBox(CComboBox& m_PortNO);///将QueryKey查询到的串口号添加到CComboBox控件中

protected:
	// protected memberfunctions
	void		ProcessErrorMessage(char* ErrorText);///错误处理
	static DWORD WINAPI CommThread(LPVOID pParam);///线程函数
	static void	ReceiveChar(CSerialPort* port);
	static void ReceiveStr(CSerialPort* port); //add by itas109 2016-06-22
	static void	WriteChar(CSerialPort* port);

	// thread
	//CWinThread*			m_Thread;
	HANDLE			    m_Thread;
	BOOL                m_bIsSuspened;///thread监视线程是否挂起

	// synchronisation objects
	CRITICAL_SECTION	m_csCommunicationSync;///临界资源
	BOOL				m_bThreadAlive;///监视线程运行标志

	// handles
	HANDLE				m_hShutdownEvent;  //stop发生的事件
	HANDLE				m_hComm;		   // 串口句柄 
	HANDLE				m_hWriteEvent;	 // write

	// Event array. 
	// One element is used for each event. There are two event handles for each port.
	// A Write event and a receive character event which is located in the overlapped structure (m_ov.hEvent).
	// There is a general shutdown when the port is closed. 
	///事件数组，包括一个写事件，接收事件，关闭事件
	///一个元素用于一个事件。有两个事件线程处理端口。
	///写事件和接收字符事件位于overlapped结构体（m_ov.hEvent）中
	///当端口关闭时，有一个通用的关闭。
	HANDLE				m_hEventArray[3];

	// structures
	OVERLAPPED			m_ov;///异步I/O
	COMMTIMEOUTS		m_CommTimeouts;///超时设置
	DCB					m_dcb;///设备控制块

	// owner window
	//CWnd*				m_pOwner;
	HWND				m_pOwner;


	// misc
	UINT				m_nPortNr;		///串口号
	char*				m_szWriteBuffer;///写缓冲区
	DWORD				m_dwCommEvents;
	DWORD				m_nWriteBufferSize;///写缓冲大小

	int				    m_nWriteSize;//写入字节数 //add by mrlong 2007-12-25
};

#endif __SERIALPORT_H__
