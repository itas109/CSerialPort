<p align="center"><strong>CSerialPort</strong></p>

<p align="center">
<a href="https://github.com/itas109/CSerialPort/releases"><img alt="Version" src="https://img.shields.io/github/release/itas109/CSerialPort"/></a>
<a href="https://github.com/itas109/CSerialPort/stargazers"><img alt="Stars" src="https://img.shields.io/github/stars/itas109/CSerialPort"/></a>
<a href="https://github.com/itas109/CSerialPort/network/members"><img alt="Forks" src="https://img.shields.io/github/forks/itas109/CSerialPort"/></a>
<a href="https://github.com/itas109/CSerialPort/blob/master/LICENSE"><img alt="License" src="https://img.shields.io/badge/License-LGPL%203.0-orange"/></a>
<img alt="language" src="https://img.shields.io/badge/language-c++-red"/>
<img alt="platform" src="https://img.shields.io/badge/platform-windows%20%7C%20linux%20%7C%20macos%20%7C%20raspberrypi%20%7C%20freebsd-lightgrey"/>
<img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/itas109/CSerialPort">
</p>

<p align="center">
语言：<a href="README-EN.md">English 英语</a> / <strong>中文</strong>
</p>

基于C++的轻量级开源跨平台串口类库，可以轻松实现跨平台多操作系统的串口读写

---
# Design Principles 设计原则

* 跨平台
* 简单易用
* 高效

# Platform 平台
CSerialPort已经在以下平台做过测试:

   - DOS ( x86_64 )
   - Windows ( x86_64 )
   - Linux ( x86_64, aarch64, mips64el )
   - macOS ( x86_64 )
   - Raspberry Pi ( armv7l )
   - FreeBSD ( x86_64 )
   - ...

# Todo List 待处理事项

## Strategic Goal 战略目标

- [x] 1.首先支持windows和linux平台
- [ ] 2.增加通用串口通信协议
- [ ] 3.支持热插拔
- [ ] 4.更高效的通知模块
- [ ] 5.支持其他语言，如C, C#, Python, Java, Golang等
- [x] 6.同步串口通信
- [ ] 7.全新的跨平台串口调试助手
- [ ] 8.增加一个类库的介绍和使用视频
- [ ] 9.串口侦听hook

## Short-term Goal 短期目标

- [x] 1.跨平台操作系统识别库
- [ ] 2.跨平台多线程类库
- [ ] 3.跨平台锁类库
- [ ] 4.跨平台高效定时器类库
- [ ] 5.性能测试报告(吞吐量、时延、丢包率)


# Last Modify 最新版本

## Version: 4.0.3.200429
by itas109 on 2020-04-29

# Quick Start 快速开始

```
$ git clone https://github.com/itas109/CSerialPort.git
$ cd CSerialPort
$ cmake -B bin .
$ cmake --build bin
```

# Tested Machine 测试机器

| 系统版本 | CPU架构 |Gui | 编译器 | 版本 | 测试时间 |
| :-----:| :----: | :----: | :----: | :----: |:----: |
| DeepIn 15.11 64bit CN | x86_64 | QT 5.12.6 | GCC 6.3.0 | 4.0.3 |2020-04-29|
| NeoKylin Server 7.0 CN| x86_64 | NoGui | GCC 4.8.5 | 4.0.3 |2020-04-29|
| Win7 Ultimate 64bit CN | x86_64 | QT 5.6.2 | MSVC2013u5 32bit | 4.0.3 |2020-04-29|
| Win7 Ultimate 64bit CN | x86_64 | QT 5.12.7 | MSVC2017 64bit |  4.0.3 |2020-04-29|
| Win7 Ultimate 64bit CN | x86_64 | NoGui | MinGW73 64bit |  4.0.3 |2020-04-29|
| Win7 Ultimate 64bit CN | x86_64 | NoGui | MinGW48 32bit |  4.0.3 |2020-04-29|
| Win7 Ultimate 64bit CN | x86_64 | MFC | MSVC2013u5 32bit |  4.0.3 |2020-04-29|
| Win7 Ultimate 64bit CN | x86_64 | MFC | MSVC2015u3 32bit |  4.0.3 |2020-04-29|
| Win10 Enterprise 64bit CN | x86_64 | MFC | MSVC2015u3 32bit | 4.0.3 |2020-04-29|
| Win10 Enterprise 64bit CN | x86_64 | QT 5.12.7 | MinGW73 64bit | 4.0.3 |2020-04-29|
| Linux raspberrypi 4.9.70 | armv7l | NoGUi | GCC 4.9.2 |  4.0.3 |2020-04-29|

# Screenshot 截图

## Gui 图形用户界面

示例路径: CSerialPort/examples/CommQT

![image](./pic/linux.jpg)

## Tui 终端用户界面 

示例路径: CSerialPort/examples/CommTui

![image](./pic/linux_tui.jpg)

## No Gui 无界面

示例路径: CSerialPort/examples/CommNoGui

![image](./pic/linux_no_gui.jpg)

# Directory List 目录列表

[目录列表文档](./doc/directory_list.md)

# Error Guide 错误指南文档

[错误指南文档](./doc/error_guide.md)

# Frequently Asked Questions 常见问题回答

[常见问题回答](./doc/FAQ.md)

# Contacting 联系方式

* Email : itas109@qq.com

* QQ群 : [129518033](http://shang.qq.com/wpa/qunwpa?idkey=2888fa15c4513e6bfb9347052f36e437d919b2377161862948b2a49576679fc6)

# Links 链接

* [CSDN博客](https://blog.csdn.net/itas109)
* [Github](https://github.com/itas109/CSerialPort)
* [Gitee码云](https://gitee.com/itas109/CSerialPort)

# CSerialPort-based Applications 基于CSerialPort的应用
## 1.[CommMaster通信大师](https://gitee.com/itas109/CommMaster)
* 支持windows/linux/macos/raspberrypi等等
* 支持自定义串口名称
* 支持自定义波特率
* 支持自定义语言
* 支持自定义主题

https://gitee.com/itas109/CommMaster

![image](./pic/CommMaster.png)

## 2.[CommLite](https://github.com/itas109/CommLite)

CommLite是一款基于CSerialPort的文本UI串口调试助手

* 支持x86, arm, mips等cpu架构
* 支持windows dos, linux, macos, raspberrypi, freebsd等操作系统

https://github.com/itas109/CommLite

![image](./pic/commlite.gif)

# Donate 捐助

[CSDN博客](https://blog.csdn.net/itas109)

---
# Other branches 其他分支

Remon Spekreijse的串口类库对于本类库有着深远的影响，十分感谢Remon Spekreijse
http://www.codeguru.com/cpp/i-n/network/serialcommunications/article.php/c2483/A-communication-class-for-serial-port.htm

仅支持windows版本分支 : 
https://github.com/itas109/CSerialPort/tree/CSerialPort_win_3.0.3

---

# License 开源协议

自 V3.0.0.171216 版本后采用[GNU Lesser General Public License v3.0](LICENSE)