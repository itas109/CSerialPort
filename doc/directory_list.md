update : 2020-04-12

```
|-- CSerialPort # root
    |-- LICENSE # LGPL3.0 license
    |-- README.md
    |-- README-EN.md
    |-- VERSION # version 版本号
    |-- Demo # example 示例目录
    |   |-- CommMFC # CSerialPort MFC Demo use source code win32直接调用源码MFC程序示例
    |   |-- CommDLL # CSerialPort MFC Demo use Win32 Dll win32动态库MFC程序示例
    |   |-- CommQT # CSerialPort QT win/linux Demo QT win/linux 程序示例
    |   |-- CommNoGui # CSerialPort No Gui win/linux Demo win/linux无界面程序示例
    |-- doc # document 文档目录
    |   |-- CSerialPort_doc_cn.chm # Chinese documnet 简体中文说明书
    |   |-- CSerialPort_doc_en.chm # English documnet 英文说明书
    |   |-- directory_list.md # directory list 目录列表
    |   |-- FAQ.md # Frequently Asked Questions 常见问题回答
    |   |-- error_guide.md # error guide 错误指南文档
    |   |-- How To Use.txt
    |   |-- suspending.txt
    |-- lib # lib 库目录
    |   |-- Linux # windows lib windows库目录
    |       |-- compile.sh # sh compile 命令编译
    |       |-- Makefile # Makefile compile Makefile编译
    |   |-- Windows # windows lib windows库目录
    |       |-- VC12 # windows lib for vs2013 windows vs2013库目录
    |           |-- libcserialport
    |               |-- libcserialport
    |   |--  CMakeLists.txt # win/linux library cmake
    |-- pic # picture 图片
    |-- src # source 源代码
        |-- osplatformutil.h # os platform define 操作系统定义
        |-- SerialPort.cpp
        |-- SerialPort.h # Lightweight library of serial port, which can easy to read and write serical port on windows and linux with C++ 轻量级跨平台串口读写类库
        |-- SerialPortBase.cpp
        |-- SerialPortBase.h # CSerialPort Base class 串口基类 
        |-- SerialPortInfo.cpp
        |-- SerialPortInfo.h # CSerialPortInfo class 串口信息辅助类 
        |-- SerialPortInfoBase.cpp
        |-- SerialPortInfoBase.h # CSerialPortInfo Base class 串口信息辅助类基类 
        |-- SerialPortInfoUnixBase.cpp
        |-- SerialPortInfoUnixBase.h # CSerialPortInfo unix class unix串口信息辅助类基类 
        |-- SerialPortInfoWinBase.cpp
        |-- SerialPortInfoWinBase.h # CSerialPortInfo windows class windows串口信息辅助类基类 
        |-- SerialPortUnixBase.cpp
        |-- SerialPortUnixBase.h # CSerialPort unix Base class unix串口基类 
        |-- SerialPortWinBase.cpp
        |-- SerialPortWinBase.h # CSerialPort Windows Base class windows串口基类 
        |-- SerialPort_global.h # Global difine of CSerialPort 串口全局定义 
        |-- sigslot.h # signal and slot 信号与槽
```