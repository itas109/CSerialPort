# CSerialPort

a lightweight library of serial port, which can easy to read and write serical port on windows and linux with C++

# License
since V3.0.0.171216 use LGPL v3 License

---

Blog : http://blog.csdn.net/itas109

QQ Group : [129518033](http://shang.qq.com/wpa/qunwpa?idkey=2888fa15c4513e6bfb9347052f36e437d919b2377161862948b2a49576679fc6)

# Last Modify
## Version: 4.0.0.181210 beta
by itas109 on 2018-12-10

http://blog.csdn.net/itas109
https://github.com/itas109

* Cross-platform design
* Simplify the use process
* higher efficiency

# Todo List

- [ ] 1.support windows and linux first
- [ ] 2.add communicating protocol
- [ ] 3.support hot swap
- [ ] 4.use new notify module
- [ ] 5.support other language,such as C, C#, Python, Java, Golang
- [ ] 6.sync serial port communication

# Test
* VS2013 update 5
* VS2015 update 3

# directory List

```
|-- CSerialPort # root
    |-- .gitattributes
    |-- .gitignore
    |-- LICENSE # LGPL3.0 license
    |-- README.md 
    |-- VERSION # version 版本号
    |-- Demo # example 示例目录
    |   |-- Comm # CSerialPort MFC Demo use source code win32直接调用源码MFC程序示例
    |   |   |-- Comm
    |   |   |-- Release # CSerialPort Release Application 可以直接运行的Release程序
    |   |       |-- Comm.exe
    |   |-- CommDLL # CSerialPort MFC Demo use Win32 Dll win32动态库MFC程序示例
    |   |   |-- Comm
    |   |-- CSerialPortDemoWin32Console # CSerialPort Demo for Win32 Console win32控制台程序示例
    |       |-- CSerialPortDemoWin32Console
    |-- doc # document 文档目录
    |   |-- CSerialPort_doc_cn.chm # Chinese documnet 简体中文说明书
    |   |-- CSerialPort_doc_en.chm # English documnet 英文说明书
    |   |-- How To Use.txt
    |   |-- suspending.txt
    |-- lib # lib 库目录
    |   |-- Windows # windows lib windows库目录
    |       |-- VC12 # windows lib for vs2013 windows vs2013库目录
    |           |-- libcserialport
    |               |-- libcserialport
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

---
# Other branches

Remon Spekreijse's serial library has had a profound impact on me, thanks again to Remon Spekreijse
http://www.codeguru.com/cpp/i-n/network/serialcommunications/article.php/c2483/A-communication-class-for-serial-port.htm


only for windows branches : 
https://github.com/itas109/CSerialPort/tree/CSerialPort_win_3.0.3

---