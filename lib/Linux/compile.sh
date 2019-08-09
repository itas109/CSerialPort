#! /bin/bash

g++ ./../../src/SerialPortInfo.cpp ./../../src/SerialPortInfoBase.cpp ./../../src/SerialPortInfoUnixBase.cpp ./../../src/SerialPort.cpp ./../../src/SerialPortBase.cpp ./../../src/SerialPortUnixBase.cpp  -I./../../src -fPIC -shared -lpthread -o libcserialport.so