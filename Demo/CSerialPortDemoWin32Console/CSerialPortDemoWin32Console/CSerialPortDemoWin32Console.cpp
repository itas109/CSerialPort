// CSerialPortDemoWin32Console.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "tchar.h"

#include "SerialPort.h"
#include <iostream>

using namespace std;
using namespace itas109;

class mySlot : public has_slots<>
{
public:
	void OnSendMessage(unsigned char* str, int port, int str_len)
	{
		std::cout << "port : " << port << ", str_len : " << str_len << ", data : " << str << endl;
	};
};

int _tmain(int argc, _TCHAR* argv[])
{
	CSerialPort m_SerialPort;
	mySlot receive;

	HWND m_handle = GetConsoleWindow();

	int port = 2;

	int i = m_SerialPort.InitPort(m_handle, port);
	m_SerialPort.StartMonitoring();

	std::cout << "port : " << port << ", IsOpened : " << m_SerialPort.IsOpened() << endl;

#ifdef _SEND_DATA_WITH_SIGSLOT
	m_SerialPort.sendMessageSignal.connect(&receive, &mySlot::OnSendMessage);
#endif

	TCHAR temp[256] = _T("Hello itas109\n");
	size_t len = _tcsclen(temp) + 1;;
	char* m_str = NULL;
	size_t* converted = 0;
	m_str = new char[len];
#ifdef UNICODE
	wcstombs_s(converted, m_str, len, temp, _TRUNCATE);
#else
	m_str = temp;
#endif
	m_SerialPort.WriteToPort(m_str, len);

	while (1);

	return 0;
}

