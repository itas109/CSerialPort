#include <iostream>

#include "sigslot.h"
#include "SerialPort.h"
#include "SerialPortInfo.h"

using namespace itas109;
using namespace std;

int countRead = 0;

//For Linux
//g++ CSerialPortDemoNoGui.cpp SerialPort.cpp SerialPortBase.cpp SerialPortUnixBase.cpp -lpthread -o CSerialPortDemoNoGui
//g++ CSerialPortDemoNoGui.cpp SerialPortInfo.cpp SerialPortInfoBase.cpp SerialPortInfoUnixBase.cpp SerialPort.cpp SerialPortBase.cpp SerialPortUnixBase.cpp -lpthread -o CSerialPortDemoNoGui

class mySlot : public has_slots<>
{
public:
    mySlot(CSerialPort & sp){ m_sp = sp; };

    void OnSendMessage()
    {
        //read
        recLen = m_sp.readAllData(str);
        cout << "receive data : " << str  << ", receive size : " << recLen<< endl;

        //write for test
        m_sp.writeData("test", 4);
        countRead++;
        cout << " +++ " << countRead << endl;
        if (countRead == 100)
        {
            cout << " --- stop " << endl;
            m_sp.close();
        }
    };

private:
    mySlot(){};

private:
    CSerialPort m_sp;

    char str[256];
    int recLen;
};

int main()
{
    CSerialPort sp;

    countRead = 0;

    list<string> m_availablePortsList;

    mySlot receive(sp);

    m_availablePortsList = CSerialPortInfo::availablePorts();

    cout << "availablePorts : " << endl;

    list<string>::iterator it;
    for(it = m_availablePortsList.begin();it != m_availablePortsList.end(); it++)
    {
        cout << *it << endl;;
    }

    if(m_availablePortsList.size() == 0)
    {
        cout << "No valid port" << endl;;
    }

    sp.init("/dev/ttyUSB0");

    sp.open();

    cout << " open status : " <<  sp.isOpened() << endl;

    //connect for read
    sp.readReady.connect(&receive, &mySlot::OnSendMessage);

    //write
    for (int i = 0; i < 50; i++)
    {
        sp.writeData("write", 5);
    }

    while (1);

    return 0;
}
