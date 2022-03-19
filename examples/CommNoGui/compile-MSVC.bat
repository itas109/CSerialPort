@ECHO OFF

echo MSVC x86 compile

echo.
echo 0 - Visual Studio 2008(VC9)
echo 1 - Visual Studio 2010(VC10)
echo 2 - Visual Studio 2012(VC11)
echo 3 - Visual Studio 2013(VC12)
echo 4 - Visual Studio 2015(VC14)
echo 5 - Visual Studio 2017(VC14.1)
echo 6 - Visual Studio 2019(VC14.2)
echo 7 - Visual Studio 2022(VC14.3)
echo.

:loop
set /p choice=please input index(0-7):
echo "%choice:"=""%"|findstr /rx "^""[0-7]""" && echo. || goto loop

if %choice%==0 (
   if exist "C:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\Tools\vsvars32.bat" (
      call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\Tools\vsvars32.bat"
   ) else if exist "D:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\Tools\vsvars32.bat" (
      call "D:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\Tools\vsvars32.bat"
   ) else (
      echo "No Found VS2008 vsvars32.bat"
   )
)else if %choice%==1 (
   if exist "C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\Tools\vsvars32.bat" (
      call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\Tools\vsvars32.bat"
   ) else if exist "D:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\Tools\vsvars32.bat" (
      call "D:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\Tools\vsvars32.bat"
   ) else (
      echo "No Found VS2010 vsvars32.bat"
   )
) else if %choice%==2 (
   if exist "C:\Program Files (x86)\Microsoft Visual Studio 11.0\Common7\Tools\vsvars32.bat" (
      call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\Common7\Tools\vsvars32.bat"
   ) else if exist "D:\Program Files (x86)\Microsoft Visual Studio 11.0\Common7\Tools\vsvars32.bat" (
      call "D:\Program Files (x86)\Microsoft Visual Studio 11.0\Common7\Tools\vsvars32.bat"
   ) else (
      echo "No Found VS2012 vsvars32.bat"
   )
) else if %choice%==3 (
   if exist "C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\vsvars32.bat" (
      call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\vsvars32.bat"
   ) else if exist "D:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\vsvars32.bat" (
      call "D:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\vsvars32.bat"
   ) else (
      echo "No Found VS2013 vsvars32.bat"
   )
) else if %choice%==4 (
   if exist "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\vsvars32.bat" (
      call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\vsvars32.bat"
   ) else if exist "D:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\vsvars32.bat" (
      call "D:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\vsvars32.bat"
   ) else (
      echo "No Found VS2015 vsvars32.bat"
   )
) else if %choice%==5 (
   if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat" (
      call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
   ) else if exist "D:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat" (
      call "D:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
   ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars32.bat" (
      call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars32.bat"
   ) else if exist "D:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars32.bat" (
      call "D:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars32.bat"
   )else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvars32.bat" (
      call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvars32.bat"
   ) else if exist "D:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvars32.bat" (
      call "D:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvars32.bat"
   ) else (
      echo "No Found VS2017 vcvars32.bat(notice not vsvars32.bat)"
   )
) else if %choice%==6 (
      if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat" (
      call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"
   ) else if exist "D:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat" (
      call "D:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"
   ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars32.bat" (
      call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars32.bat"
   ) else if exist "D:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars32.bat" (
      call "D:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars32.bat"
   )else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars32.bat" (
      call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars32.bat"
   ) else if exist "D:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars32.bat" (
      call "D:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars32.bat"
   ) else (
      echo "No Found VS2019 vcvars32.bat(notice not vsvars32.bat)"
   )
) else if %choice%==7 (
      if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat" (
      call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"
   ) else if exist "D:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat" (
      call "D:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"
   ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars32.bat" (
      call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars32.bat"
   ) else if exist "D:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars32.bat" (
      call "D:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars32.bat"
   )else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars32.bat" (
      call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars32.bat"
   ) else if exist "D:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars32.bat" (
      call "D:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars32.bat"
   ) else (
      echo "No Found VS2022 vcvars32.bat(notice not vsvars32.bat)"
   )
)

rem debug compile
rem cl /ZI /nologo /W3 /WX- /Od /Oy- /D WIN32 /D _DEBUG /D _CONSOLE /D _LIB /D _UNICODE /D UNICODE /Gm /EHsc /RTC1 /MDd /GS /fp:precise /Zc:wchar_t /Zc:forScope  /Gd /TP /I "..\..\include" ..\..\src\SerialPort.cpp ..\..\src\SerialPortBase.cpp ..\..\src\SerialPortInfo.cpp ..\..\src\SerialPortInfoBase.cpp ..\..\src\SerialPortInfoWinBase.cpp ..\..\src\SerialPortWinBase.cpp CSerialPortDemoNoGui.cpp /link /OUT:"CSerialPortDemoNoGui-MSVC.exe" /INCREMENTAL /NOLOGO advapi32.lib setupapi.lib /SUBSYSTEM:CONSOLE /MACHINE:X86

rem release compile
cl /Zi /nologo /W3 /WX- /O2 /Oi /Oy- /GL /D WIN32 /D NDEBUG /D _CONSOLE /D _LIB /D _UNICODE /D UNICODE /Gm- /EHsc /MD /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Gd /TP /I "..\..\include" ..\..\src\SerialPort.cpp ..\..\src\SerialPortBase.cpp ..\..\src\SerialPortInfo.cpp ..\..\src\SerialPortInfoBase.cpp ..\..\src\SerialPortInfoWinBase.cpp ..\..\src\SerialPortWinBase.cpp CSerialPortDemoNoGui.cpp /link /OUT:"CSerialPortDemoNoGui-MSVC.exe" /INCREMENTAL /NOLOGO advapi32.lib setupapi.lib /SUBSYSTEM:CONSOLE /MACHINE:X86

pause