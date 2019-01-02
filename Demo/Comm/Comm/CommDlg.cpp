
// CommDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Comm.h"
#include "CommDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int BaudRateArray[] = { 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 56000, 57600, 115200 };

string ParityArray[] = { "None", "Odd", "Even", "Mark", "Space" };

string DataBitsArray[] = { "5", "6", "7","8" };

string StopArray[] = { "1", "1.5", "2"};

CSerialPort m_SerialPort;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCommDlg 对话框



CCommDlg::CCommDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCommDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCommDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PORT_Nr, m_PortNr);
	DDX_Control(pDX, IDC_COMBO_BAUDRATE, m_BaudRate);
	DDX_Control(pDX, IDC_BUTTON_OPEN_CLOSE, m_OpenCloseCtrl);
	DDX_Control(pDX, IDC_SendEdit, m_Send);
	DDX_Control(pDX, IDC_ReceiveEdit, m_ReceiveCtrl);
	DDX_Control(pDX, IDC_STATIC_RECV_COUNT_VALUE, m_recvCountCtrl);
	DDX_Control(pDX, IDC_STATIC_SEND_COUNT_VALUE, m_sendCountCtrl);
	DDX_Control(pDX, IDC_COMBO_PARITY, m_Parity);
	DDX_Control(pDX, IDC_COMBO_STOP, m_Stop);
	DDX_Control(pDX, IDC_COMBO_DATABITS, m_DataBits);
}

BEGIN_MESSAGE_MAP(CCommDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_CLOSE, &CCommDlg::OnBnClickedButtonOpenClose)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CCommDlg::OnBnClickedButtonSend)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CCommDlg::OnBnClickedButtonClear)
END_MESSAGE_MAP()


// CCommDlg 消息处理程序

BOOL CCommDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	rx = 0;
	tx = 0;
	m_recvCountCtrl.SetWindowText(CString("0"));
	m_sendCountCtrl.SetWindowText(CString("0"));

	CString temp;
	//添加波特率到下拉列表
	for (int i = 0; i < sizeof(BaudRateArray) / sizeof(int); i++)
	{
		temp.Format(_T("%d"), BaudRateArray[i]);
		m_BaudRate.InsertString(i, temp);
	}

	temp.Format(_T("%d"), 9600);
	m_BaudRate.SetCurSel(m_BaudRate.FindString(0, temp));

	//校验位
	for (int i = 0; i < sizeof(ParityArray) / sizeof(string); i++)
	{
#ifdef UNICODE
		temp.Format(_T("%S"), ParityArray[i].c_str());
#else
		temp.Format(_T("%s"), ParityArray[i].c_str());
#endif
		m_Parity.InsertString(i, temp);
	}
	m_Parity.SetCurSel(0);

	//数据位
	for (int i = 0; i < sizeof(DataBitsArray) / sizeof(string); i++)
	{
#ifdef UNICODE
		temp.Format(_T("%S"), DataBitsArray[i].c_str());
#else
		temp.Format(_T("%s"), DataBitsArray[i].c_str());
#endif
		m_DataBits.InsertString(i, temp);
	}
	m_DataBits.SetCurSel(3);

	//停止位
	for (int i = 0; i < sizeof(StopArray) / sizeof(string); i++)
	{
#ifdef UNICODE
		temp.Format(_T("%S"), StopArray[i].c_str());
#else
		temp.Format(_T("%s"), StopArray[i].c_str());
#endif
		m_Stop.InsertString(i, temp);
	}
	m_Stop.SetCurSel(0);

	//获取串口号
	list<string> m_portsList = CSerialPortInfo::availablePorts();
	list<string>::iterator itor;
	TCHAR m_regKeyValue[255];
	for (itor = m_portsList.begin(); itor != m_portsList.end(); ++itor)
	{
#ifdef UNICODE
		int iLength;
		const char * _char = (*itor).c_str();
		iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
		MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, m_regKeyValue, iLength);
#else
		strcpy_s(m_regKeyValue, 255, (*itor).c_str());
#endif
		m_PortNr.AddString(m_regKeyValue);
	}
	m_PortNr.SetCurSel(0);
	
	OnBnClickedButtonOpenClose();

	m_Send.SetWindowText(_T("http://blog.csdn.net/itas109"));

	m_SerialPort.readReady.connect(this, &CCommDlg::OnReceive);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCommDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCommDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCommDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCommDlg::OnSendMessage(unsigned char* str, int port, int str_len)
{
	CString str1((char*)str);

	m_ReceiveCtrl.SetSel(-1, -1);
	m_ReceiveCtrl.ReplaceSel(str1);
}

LRESULT CCommDlg::OnReceiveStr(WPARAM str, LPARAM commInfo)
{
	struct serialPortInfo
	{
		UINT portNr;//串口号
		DWORD bytesRead;//读取的字节数
	}*pCommInfo;
	pCommInfo = (serialPortInfo*)commInfo;

	CString str1((char*)str);
	//char* m_str = (char*)str;
	//TCHAR* dest = NULL;
	//dest = new TCHAR[sizeof(TCHAR) * pCommInfo->bytesRead];
	//wmemcpy(dest, (TCHAR*)str, sizeof(TCHAR)* pCommInfo->bytesRead+1);
	
	//获取指定串口的数据
	//if (pCommInfo->portNr == 2)
	//{
			m_ReceiveCtrl.SetSel(-1, -1);
			m_ReceiveCtrl.ReplaceSel(str1);
	//}
	return TRUE;
}

void CCommDlg::OnBnClickedButtonOpenClose()
{
	// TODO:  在此添加控件通知处理程序代码
	//GetDlgItem(IDC_SendEdit)->SetFocus();
	CString temp;
	m_OpenCloseCtrl.GetWindowText(temp);///获取按钮的文本
	UpdateData(true);
	if (temp == _T("关闭串口"))///表示点击后是"关闭串口"，也就是已经关闭了串口
	{
		m_SerialPort.close();
		m_OpenCloseCtrl.SetWindowText(_T("打开串口"));///设置按钮文字为"打开串口"
	}
	///打开串口操作
	else if (m_PortNr.GetCount() > 0)///当前列表的内容个数
	{
		string portName;
		int SelBaudRate;
		int SelParity;
		int SelDataBits;
		int SelStop;

		UpdateData(true);
		m_PortNr.GetWindowText(temp);///CString temp
#ifdef UNICODE
		portName = CW2A(temp.GetString());
#else
		portName = temp.GetBuffer();
#endif	

		m_BaudRate.GetWindowText(temp);
		SelBaudRate = _tstoi(temp);	

		SelParity = m_Parity.GetCurSel();

		m_DataBits.GetWindowText(temp);
		SelDataBits = _tstoi(temp);

		SelStop = m_Stop.GetCurSel();

		m_SerialPort.init(portName, SelBaudRate, itas109::Parity(SelParity), itas109::DataBits(SelDataBits), itas109::StopBits(SelStop));
		m_SerialPort.open();

		if (m_SerialPort.isOpened())
		{
			m_OpenCloseCtrl.SetWindowText(_T("关闭串口"));
		}
		else
		{
			AfxMessageBox(_T("串口已被占用！"));
		}
	}
	else
	{
		AfxMessageBox(_T("没有发现串口！"));
	}
}


void CCommDlg::OnBnClickedButtonSend()
{
	// TODO:  在此添加控件通知处理程序代码
	GetDlgItem(IDC_SendEdit)->SetFocus();
	CString temp;
	UpdateData(true);
	m_OpenCloseCtrl.GetWindowText(temp);
	if (temp == "打开串口")///没有打开串口
	{
		AfxMessageBox(_T("请首先打开串口"));
		return;
	}

	m_Send.GetWindowText(temp);
	size_t len = _tcsclen(temp) + 1;;
	char* m_str = NULL;
	size_t* converted = 0;
	m_str = new char[len];
#ifdef UNICODE
	wcstombs_s(converted, m_str, len, temp.GetBuffer(0), _TRUNCATE);
#else
	m_str = temp.GetBuffer(0);
#endif
	
	m_SerialPort.writeData(m_str, len);

	tx += len - 1;

	CString str2;
	str2.Format(_T("%d"), tx);
	m_sendCountCtrl.SetWindowText(str2);
}


void CCommDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_SerialPort.close();
	CDialogEx::OnClose();
}

void CCommDlg::OnReceive()
{
	char * str = NULL;
	str = new char[256];
	m_SerialPort.readAllData(str);

	CString str1((char*)str);

	rx += str1.GetLength();

	m_ReceiveCtrl.SetSel(-1, -1);
	m_ReceiveCtrl.ReplaceSel(str1);

	CString str2;
	str2.Format(_T("%d"), rx);
	m_recvCountCtrl.SetWindowText(str2);
}


void CCommDlg::OnBnClickedButtonClear()
{
	// TODO:  在此添加控件通知处理程序代码
	rx = 0;
	tx = 0;
	m_recvCountCtrl.SetWindowText(CString("0"));
	m_sendCountCtrl.SetWindowText(CString("0"));
}
