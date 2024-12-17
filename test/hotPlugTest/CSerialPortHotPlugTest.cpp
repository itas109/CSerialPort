#include <stdio.h>

#include "CSerialPort/SerialPortHotPlug.hpp"

class MyListener : public itas109::CSerialPortHotPlugListener
{
public:
    MyListener(){}

    void onHotPlugEvent(const char *portName, int isAdd)
    {
        printf("portName: %s, isAdded: %d\n", portName, isAdd);
    }
};

int main(int argc, char *argv[])
{
    MyListener listener;

    itas109::CSerialPortHotPlug m_serialPortHotPlug;
	m_serialPortHotPlug.connectHotPlugEvent(&listener);
    printf("CSerialPortHotPlug...\n");
	
	while (true);

    return 0;
}