@ECHO OFF

echo MSVC120 compile"

call "D:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\vsvars32.bat"

rem debug compile
rem cl /ZI /nologo /W3 /WX- /Od /Oy- /D WIN32 /D _DEBUG /D _CONSOLE /D _LIB /D _UNICODE /D UNICODE /Gm /EHsc /RTC1 /MDd /GS /fp:precise /Zc:wchar_t /Zc:forScope  /Gd /TP /I "E:\Git\CSerialPort\src" ..\..\src\SerialPort.cpp ..\..\src\SerialPortBase.cpp ..\..\src\SerialPortInfo.cpp ..\..\src\SerialPortInfoBase.cpp ..\..\src\SerialPortInfoWinBase.cpp ..\..\src\SerialPortWinBase.cpp CSerialPortDemoNoGui.cpp /link /OUT:"CSerialPortDemoNoGui-MSVC.exe" /INCREMENTAL /NOLOGO user32.lib advapi32.lib /SUBSYSTEM:CONSOLE /MACHINE:X86

rem release compile
cl /Zi /nologo /W3 /WX- /O2 /Oi /Oy- /GL /D WIN32 /D NDEBUG /D _CONSOLE /D _LIB /D _UNICODE /D UNICODE /Gm- /EHsc /MD /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Gd /TP /I "E:\Git\CSerialPort\src" ..\..\src\SerialPort.cpp ..\..\src\SerialPortBase.cpp ..\..\src\SerialPortInfo.cpp ..\..\src\SerialPortInfoBase.cpp ..\..\src\SerialPortInfoWinBase.cpp ..\..\src\SerialPortWinBase.cpp CSerialPortDemoNoGui.cpp /link /OUT:"CSerialPortDemoNoGui-MSVC.exe" /INCREMENTAL /NOLOGO user32.lib advapi32.lib setupapi.lib /SUBSYSTEM:CONSOLE /MACHINE:X86

pause