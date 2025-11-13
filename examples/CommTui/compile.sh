#! /bin/bash

g++ -std=c++11 CSerialPortDemoTui.cpp tui.c ../../src/SerialPortInfo.cpp ../../src/SerialPortInfoBase.cpp ../../src/SerialPortInfoUnixBase.cpp ../../src/SerialPort.cpp ../../src/SerialPortBase.cpp ../../src/SerialPortUnixBase.cpp -lpthread -o CSerialPortDemoTui -I. -I../../include -L. -lncurses
