
// CommDlg.h : 头文件
//

#pragma once

//About CSerialPort start
#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
using namespace itas109;
//About CSerialPort end

using namespace std;


// CCommDlg 对话框

	class CCommDlg : public CDialogEx, public has_slots<>//About CSerialPort 
{
// 构造
public:
	CCommDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_COMM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	void OnSendMessage(unsigned char* str, int port, int str_len);
	void OnReceive();//About CSerialPort 

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnReceiveStr(WPARAM str, LPARAM commInfo);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CComboBox m_PortNr;
	CComboBox m_BaudRate;
	afx_msg void OnBnClickedButtonOpenClose();
	CButton m_OpenCloseCtrl;
	afx_msg void OnBnClickedButtonSend();
	CEdit m_Send;
	CEdit m_ReceiveCtrl;
	afx_msg void OnClose();
//	LONGLONG m_recvCount;
	CStatic m_recvCountCtrl;
	CStatic m_sendCountCtrl;
	afx_msg void OnBnClickedButtonClear();

private:
	CSerialPort m_SerialPort;//About CSerialPort 

	int rx; 
	int tx;
public:
	CComboBox m_Parity;
	CComboBox m_Stop;
	CComboBox m_DataBits;
	};
