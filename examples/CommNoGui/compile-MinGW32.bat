@ECHO OFF

set path=D:\Qt\Qt5.6.3\Tools\mingw492_32\bin;%path%

g++ --version

echo "MinGW compile"

g++ -std=c++11 -DCSERIALPORT_DEBUG CSerialPortDemoNoGui.cpp ../../src/SerialPortInfo.cpp ../../src/SerialPortInfoBase.cpp ../../src/SerialPortInfoWinBase.cpp ../../src/SerialPort.cpp ../../src/SerialPortBase.cpp ../../src/SerialPortWinBase.cpp -lpthread -luser32 -ladvapi32 -lsetupapi -I../../include -o CSerialPortDemoNoGui-MinGW

pause