#! /bin/bash

g++ CSerialPortDemoNoGui.cpp ../../src/SerialPortInfo.cpp ../../src/SerialPortInfoBase.cpp ../../src/SerialPortInfoUnixBase.cpp ../../src/SerialPort.cpp ../../src/SerialPortBase.cpp ../../src/SerialPortUnixBase.cpp -lpthread -I../../include -o CSerialPortDemoNoGui
