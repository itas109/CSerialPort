// CommMFCDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CommMFC.h"
#include "CommMFCDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCommMFCDlg 对话框




CCommMFCDlg::CCommMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCommMFCDlg::IDD, pParent)
	, m_ReceiveTimeoutMS(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCommMFCDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
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
    DDX_Text(pDX, IDC_EDIT_RECEIVE_TIMEOUT_MS, m_ReceiveTimeoutMS);
    DDV_MinMaxUInt(pDX, m_ReceiveTimeoutMS, 0, 999999);
    DDX_Control(pDX, IDC_EDIT_RECEIVE_TIMEOUT_MS, m_ReceiveTimeoutMSCtrl);
    DDX_Control(pDX, IDC_CHECK_DTR, m_dtrCtrl);
    DDX_Control(pDX, IDC_CHECK_RTS, m_rtsCtrl);
}

BEGIN_MESSAGE_MAP(CCommMFCDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_CLOSE, &CCommMFCDlg::OnBnClickedButtonOpenClose)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CCommMFCDlg::OnBnClickedButtonSend)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CCommMFCDlg::OnBnClickedButtonClear)
    ON_BN_CLICKED(IDC_CHECK_DTR, &CCommMFCDlg::OnBnClickedCheckDtr)
    ON_BN_CLICKED(IDC_CHECK_RTS, &CCommMFCDlg::OnBnClickedCheckRts)
    END_MESSAGE_MAP()


// CCommMFCDlg 消息处理程序

BOOL CCommMFCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	rx = 0;
	tx = 0;
	m_recvCountCtrl.SetWindowText(CString("0"));
	m_sendCountCtrl.SetWindowText(CString("0"));

	// 默认接收超时时间(毫秒)
	m_ReceiveTimeoutMSCtrl.SetWindowText(_T("0"));

	CString temp;
	//添加波特率到下拉列表
    int BaudRateArray[] = {300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 56000, 57600, 115200};
	for (int i = 0; i < sizeof(BaudRateArray) / sizeof(int); i++)
	{
		temp.Format(_T("%d"), BaudRateArray[i]);
		m_BaudRate.InsertString(i, temp);
	}

	temp.Format(_T("%d"), 9600);
	m_BaudRate.SetCurSel(m_BaudRate.FindString(0, temp));

	//校验位
    std::string ParityArray[] = {"None", "Odd", "Even", "Mark", "Space"};
	for (int i = 0; i < sizeof(ParityArray) / sizeof(std::string); i++)
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
    std::string DataBitsArray[] = {"5", "6", "7", "8"};
	for (int i = 0; i < sizeof(DataBitsArray) / sizeof(std::string); i++)
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
    std::string StopArray[] = {"1", "1.5", "2"};
	for (int i = 0; i < sizeof(StopArray) / sizeof(std::string); i++)
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
	std::vector<SerialPortInfo> m_portsList = CSerialPortInfo::availablePortInfos();
	TCHAR m_regKeyValue[256];
	for (size_t i = 0; i < m_portsList.size(); i++)
	{
#ifdef UNICODE
		int iLength;
		const char * _char = m_portsList[i].portName;
		iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
		MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, m_regKeyValue, iLength);
#else
		strcpy_s(m_regKeyValue, 256, m_portsList[i].portName);
#endif
		m_PortNr.AddString(m_regKeyValue);
	}
	m_PortNr.SetCurSel(0);
	
	//OnBnClickedButtonOpenClose();

	m_Send.SetWindowText(_T("https://blog.csdn.net/itas109"));

	m_SerialPort.connectReadEvent(this);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCommMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCommMFCDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCommMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCommMFCDlg::OnBnClickedButtonOpenClose()
{
    if (m_SerialPort.isOpen())
	{
		m_SerialPort.close();
		m_OpenCloseCtrl.SetWindowText(_T("打开串口"));///设置按钮文字为"打开串口"
	}
	///打开串口操作
	else if (m_PortNr.GetCount() > 0)///当前列表的内容个数
	{
        char portName[256] = {0};
		int SelBaudRate;
		int SelParity;
		int SelDataBits;
		int SelStop;

		UpdateData(true);
        CString temp;
		m_PortNr.GetWindowText(temp);
#ifdef UNICODE
		strcpy_s(portName, 256, CW2A(temp.GetString()));
#else
		strcpy_s(portName, 256, temp.GetBuffer());
#endif	

		m_BaudRate.GetWindowText(temp);
		SelBaudRate = _tstoi(temp);	

		SelParity = m_Parity.GetCurSel();

		m_DataBits.GetWindowText(temp);
		SelDataBits = _tstoi(temp);

		SelStop = m_Stop.GetCurSel();

        m_SerialPort.setReadIntervalTimeout(m_ReceiveTimeoutMS);
        m_SerialPort.init(portName, SelBaudRate, itas109::Parity(SelParity), itas109::DataBits(SelDataBits), itas109::StopBits(SelStop));
		m_SerialPort.open();

		if (m_SerialPort.isOpen())
		{
			m_OpenCloseCtrl.SetWindowText(_T("关闭串口"));
		}
		else
		{
            m_OpenCloseCtrl.SetWindowText(_T("打开串口"));
			AfxMessageBox(_T("串口已被占用！"));
		}
	}
	else
	{
		AfxMessageBox(_T("没有发现串口！"));
	}
}


void CCommMFCDlg::OnBnClickedButtonSend()
{
	GetDlgItem(IDC_SendEdit)->SetFocus();
    if (!m_SerialPort.isOpen()) ///没有打开串口
	{
		AfxMessageBox(_T("请首先打开串口"));
		return;
	}

	CString temp;
	m_Send.GetWindowText(temp);
	int len = 0;
	char* m_str = NULL;
#ifdef UNICODE
	// 兼容中文
	CStringA strA(temp);
	len = strA.GetLength();
	m_str = strA.GetBuffer();
#else
	len = temp.GetLength();
	m_str = temp.GetBuffer(0);
#endif
	
	m_SerialPort.writeData(m_str, len);

	tx += len;

	CString str2;
	str2.Format(_T("%d"), tx);
	m_sendCountCtrl.SetWindowText(str2);
}


void CCommMFCDlg::OnClose()
{
	m_SerialPort.close();
	CDialog::OnClose();
}

void CCommMFCDlg::onReadEvent(const char *portName, unsigned int readBufferLen)
{
    if (readBufferLen > 0)
    {
        char *data = new char[readBufferLen + 1]; // '\0'

		if (data)
        {
            int recLen = m_SerialPort.readData(data, readBufferLen);

            if (recLen > 0)
            {
                data[recLen] = '\0';

                CString str1(data);

                rx += str1.GetLength();

                m_ReceiveCtrl.SetSel(-1, -1);
                m_ReceiveCtrl.ReplaceSel(str1);

                CString str2;
                str2.Format(_T("%d"), rx);
                m_recvCountCtrl.SetWindowText(str2);
            }

			delete[] data;
            data = NULL;
		}
	}
}


void CCommMFCDlg::OnBnClickedButtonClear()
{
	rx = 0;
	tx = 0;
	m_recvCountCtrl.SetWindowText(CString("0"));
	m_sendCountCtrl.SetWindowText(CString("0"));
}

void CCommMFCDlg::OnBnClickedCheckDtr()
{
    if(BST_CHECKED == m_dtrCtrl.GetCheck())
    {
        m_SerialPort.setDtr(true);
    }
    else
    {
        m_SerialPort.setDtr(false);
	}
}

void CCommMFCDlg::OnBnClickedCheckRts()
{
    if (BST_CHECKED == m_rtsCtrl.GetCheck())
    {
        m_SerialPort.setRts(true);
    }
    else
    {
        m_SerialPort.setRts(false);
    }
}
