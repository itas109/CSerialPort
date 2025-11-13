@ECHO OFF

set path=D:\Program Files\LLVM\bin\;%path%

REM clang version 18.1.8

clang++ --version

echo "Clang compile"

clang++ CSerialPortDemoNoGui.cpp ../../src/SerialPortInfo.cpp ../../src/SerialPortInfoBase.cpp ../../src/SerialPortInfoWinBase.cpp ../../src/SerialPort.cpp ../../src/SerialPortBase.cpp ../../src/SerialPortWinBase.cpp -luser32 -ladvapi32 -lsetupapi -I../../include -o CSerialPortDemoNoGui-Clang.exe

pause