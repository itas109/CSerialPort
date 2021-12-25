@ECHO OFF

set path=D:\Qt\Qt5.12.9\Tools\mingw730_64\bin;%path%

echo "MinGW compile"

g++ CSerialPortDemoNoGui.cpp ../../src/SerialPortInfo.cpp ../../src/SerialPortInfoBase.cpp ../../src/SerialPortInfoWinBase.cpp ../../src/SerialPort.cpp ../../src/SerialPortBase.cpp ../../src/SerialPortWinBase.cpp -lpthread -lsetupapi -I../../include -o CSerialPortDemoNoGui-MinGW

pause