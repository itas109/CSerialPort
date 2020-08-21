g++ CSerialPortDemoTui.cpp tui.c ../../src/SerialPortInfo.cpp ../../src/SerialPortInfoBase.cpp ../../src/SerialPortInfoWinBase.cpp ../../src/SerialPort.cpp ../../src/SerialPortBase.cpp ../../src/SerialPortWinBase.cpp -lpthread -lsetupapi -o CSerialPortDemoTui -I. -I../../include -L. -lpdcurses

@echo ping 127.0.0.1 -n 3 >nul

@echo exit

pause
