@ECHO OFF

echo "MinGW compile"

g++ CSerialPortDemoNoGui.cpp ../../src/SerialPortInfo.cpp ../../src/SerialPortInfoBase.cpp ../../src/SerialPortInfoWinBase.cpp ../../src/SerialPort.cpp ../../src/SerialPortBase.cpp ../../src/SerialPortWinBase.cpp -lpthread -lsetupapi -I../../src -o CSerialPortDemoNoGui-MinGW

pause