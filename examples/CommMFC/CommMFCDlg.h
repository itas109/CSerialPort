// CommMFCDlg.h : 头文件
//

#pragma once

//About CSerialPort start
#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
#include "afxwin.h"
using namespace itas109;
//About CSerialPort end


// CCommMFCDlg 对话框
class CCommMFCDlg : public CDialog, public has_slots<>//About CSerialPort 
{
// 构造
public:
	CCommMFCDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_COMMMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	void OnReceive();//About CSerialPort
	void OnReceiveBusiness(); // receive business


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

public:
	CComboBox m_PortNr;
	CComboBox m_BaudRate;
	CComboBox m_Parity;
	CComboBox m_Stop;
	CComboBox m_DataBits;
	CButton m_OpenCloseCtrl;
	CEdit m_Send;
	CEdit m_ReceiveCtrl;
	CStatic m_recvCountCtrl;
	CStatic m_sendCountCtrl;
	CEdit m_ReceiveTimeoutMSCtrl;
	UINT m_ReceiveTimeoutMS;

	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonOpenClose();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedButtonClear();

private:
	CSerialPort m_SerialPort;//About CSerialPort 

	int rx; 
	int tx;

	BOOL m_isTimerRunning;	
};
