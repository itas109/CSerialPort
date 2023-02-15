# CSerialPort Changelog

---

## v4.3.0 (2023-02-15)

Feature:
* readEvent replace with CSerialPortListener 使用CSerialPortListener进行读取事件通知
* replace std::string with const char* as much as possible 尽可能使用const char*代替std::string
* add function getReadBufferUsedLen() to get used buffer length 增加getReadBufferUsedLen函数用于获取缓冲区长度
* add LOG_INFO to output CSerialPort info 增加LOG_INFO输出串口信息

## v4.2.2 (2023-02-15)

Feature:
* read event notify with portName and readBufferLen 读取事件通知支持串口名称与长度
* add function getReadBufferUsedLen() to get used buffer length 增加getReadBufferUsedLen函数用于获取缓冲区长度
* add LOG_INFO to output CSerialPort info 增加LOG_INFO输出串口信息

## v4.2.1 (2022-11-07)

Feature:
* read buffer size deafult 4096 Bytes 读取缓冲区大小默认为4096字节
* read interval timeout default 0ms  读取超时间隔默认0ms，即实时接收
* vcpkg install CSerialPort 支持vcpkg安装CSerialPort
* wxWidgets demo 新增wxWidgets示例程序

Experimental:
* new notify class CSerialPortListener(USE_CSERIALPORT_LISTENER) 新的事件通知类CSerialPortListener(宏定义USE_CSERIALPORT_LISTENER开启)
* CSerialPort for C#(CSharp)  支持C#(CSharp)调用CSerialPort
* CSerialPort for Java 支持Java调用CSerialPort
* CSerialPort for Python 支持Python调用CSerialPort
* CSerialPort for JavaScript 支持JavaScript调用CSerialPort

## v4.2.0 (2022-10-01)

Fixed:
* #60 application compiled with source code under windows still has export information 在windows下以源码方式编译仍然带有导出信息

Feature:
* modify examples 优化示例程序
* add cross compile toochain.cmake for arm aarch64 mips64el riscv 增加arm aarch64 mips64el riscv的交叉编译cmake 文件
* improve windows receive performance 优化windows接收性能
* add custom baud rate for linux 新增linux下设置自定义波特率
* add cross-platform thread library 新增跨平台线程库
* add cross-platform ringbuffer library 新增跨平台环形缓冲区库
* add cross-platform timer based condition variable 新增基于条件变量的跨平台定时器库
* add ringbuffer for receive data(asynchronous mode) 新增环形缓冲区接收数据(异步模式)
* add read interval timeout setting(default 50ms, asynchronous mode) 新增读取超时设置(默认50ms, 异步模式)
* add clang-tidy 增加clang-tidy代码检测

## v4.1.1 (2021-09-03)

Fixed:
* #49 function writeData hanle leak on windows 修复windows下writeData函数句柄泄漏问题
* #41 could not enum all avaiable ports on windows 修复windows下偶尔枚举可用串口不全的问题
* #42 high cpu usage problem on unix 修复unix上高cpu占用问题
* #33 No Gui Application without endless loop crash problem 修复NoGui程序崩溃问题
* #28 VS2015 x64 MFC not work 修复VS2015生成x64程序不能正常运行问题

Feature:
* use unsigned int instead of int64 使用unsigned int代替int64
* add unix virtual serial port 增加unix虚拟串口工具
* read thread optimization 读取线程优化

## 4.1.0 (2020-10-10)

Fixed:
* #29 windows xp unable to locate the program input point in msvcrt.dll 无法定位程序输入点于msvcrt.dll
* #30 _T() cannot convert 'const char*' to 'LPCWSTR
* #39 fix getPortInfoList crash on unix(not linux and mac os) 修复unix系统(非linux和macos)getPortInfoList引起的崩溃问题
* #40 fix vs2008 vs2010 Cannot open include file: 'ntddser.h' 修复msvc无法找到ntddser.h问题

Feature:
* header files is separated into include directory 头文件独立到include文件夹
* add Tui Demo based pdcurses and ncurses 增加基于pdcurses和ncurses的tui示例
* use cmake compile CSerialPort 使用cmake编译
* add cmake install 增加cmake安装
* add cppcheck file 增加cppcheck代码检测文件
* add clang-format 增加clang-format代码格式化
* add travis ci and appveyor ci 增加travis和appveyor持续集成

Remove:
* remove function init of integer port 移除init整型串口函数
* remove function availablePorts and availableFriendlyPorts 移除availablePorts和availablePorts函数

## 4.0.3 (2020-04-29)

Fixed:
* fixed memory leak 修复内存泄露问题
* Optimize function closePort under windows 优化windows下的closePort函数
* #21 typo setFlowControl
* #22 function CSerialPortWinBase::openPort error when set error parameter
* #24 sigslot can not define static
* #26 linux receive miss 0x11 0x13 0x0d
* fixed compile error when baudrate not difine 修复波特率未定义错误

Feature:
* support mingw 支持mingw 4.8.2
* support mac 支持mac 10.13
* add Common baud rate 增加波特率
* add test case by gtest 增加测试用例
* add function CSerialPortInfo::availablePortInfos 增加通用串口信息枚举函数
* support linux list ports add /dev/pts/* 支持linux虚拟串口

---
## 4.0.2 (2020-01-08)

基础稳定版本
base and stable version

---
## 4.0.1 beta (2019-07-30)

测试第一版
first beta version

