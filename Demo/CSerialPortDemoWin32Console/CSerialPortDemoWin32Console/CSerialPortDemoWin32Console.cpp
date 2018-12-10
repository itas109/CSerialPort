// SerialPortDemo.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"

#include <iostream>

#include "sigslot.h"

#include "SerialPort.h"
//#pragma comment(lib,"CSerialPort.lib")

using namespace itas109;
using namespace std;

CSerialPort sp;
int countRead = 0;

class mySlot : public has_slots<>
{
public:
	void OnSendMessage()
	{
		//std::cout << "port : " << port << ", str_len : " << str_len << ", data : " << str << endl;
		sp.writeData("test", 4);

		countRead++;

		cout << " +++ " << countRead << endl;

		if (countRead == 100)
		{
			cout << " --- stop " << endl;
			sp.close();
		}
	};
};

int main()
{
	countRead = 0;

	mySlot receive;

	sp.init("COM2");

	sp.open();

	cout << sp.isOpened() << endl;

	sp.readReady.connect(&receive, &mySlot::OnSendMessage);

	for (int i = 0; i < 50; i++)
	{
		sp.writeData("write", 5);
	}

	while (1);

	return 0;
}