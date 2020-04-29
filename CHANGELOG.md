# CSerialPort Changelog


---
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

