# [CSerialPort](https://github.com/itas109/CSerialPort)

[English](README.md)

CSerialPort是一个基于C/C++的轻量级开源跨平台串口类库，可以轻松实现跨平台多操作系统的串口读写，同时还支持C#, Java, Python, Node.js等。

<p>
<a href="https://github.com/itas109/CSerialPort/releases"><img alt="Version" src="https://img.shields.io/github/release/itas109/CSerialPort"/></a>
<a href="https://github.com/itas109/CSerialPort/stargazers"><img alt="Stars" src="https://img.shields.io/github/stars/itas109/CSerialPort"/></a>
<a href="https://gitee.com/itas109/CSerialPort"><img alt="Stars" src="https://gitee.com/itas109/CSerialPort/badge/star.svg?theme=dark"/></a>
<a href="https://github.com/itas109/CSerialPort/blob/master/LICENSE"><img alt="License" src="https://img.shields.io/badge/License-LGPL%203.0-orange"/></a>
<img alt="language" src="https://img.shields.io/badge/language-c++-red"/>
<img alt="appveyor-ci" src="https://ci.appveyor.com/api/projects/status/a4t6ddubhns561kh?svg=true"/>
<!-- <img alt="travis-ci" src="https://www.travis-ci.org/itas109/CSerialPort.svg?branch=master"/> -->
</p>

---

# Design Principles 设计原则

- 跨平台
- 简单易用
- 高效

# Platform 平台

CSerialPort已经在以下平台做过测试

- Windows ( x86, x86_64, arm64 )
- Linux ( x86, x86_64, arm, arm64/aarch64, mips64el, riscv, s390x, ppc64le )
- macOS ( x86_64 )
- Raspberry Pi ( armv7l )
- FreeBSD ( x86_64 )
- ...

# Todo List 待处理事项

## Long-term Goal 长期目标

- [x] 1.首先支持windows和linux平台
- [ ] 2.增加通用串口通信协议
- [x] 3.支持热插拔 - [CSerialPortExtend](https://github.com/itas109/CSerialPortExtend)
- [x] 4.更高效的通知模块 - CSerialPortListener
- [x] 5.支持其他语言 - C#, Python, Java, Node.js - 详见[bindings](https://github.com/itas109/CSerialPort/tree/master/bindings)
- [x] 6.同步串口通信
- [x] 7.全新的跨平台串口调试助手 - [CommMaster通信大师](https://gitee.com/itas109/CommMaster)
- [x] 8.增加CSerialPort的介绍和使用教程 - [CSerialPort教程](https://blog.csdn.net/itas109/category_11520540.html)
- [ ] 9.串口侦听hook

## Short-term Goal 短期目标

- [x] 1.跨平台操作系统识别库
- [x] 2.跨平台多线程类库
- [x] 3.跨平台锁类库
- [x] 4.跨平台高效定时器类库
- [ ] 5.跨平台线程池库
- [ ] 6.性能测试报告(吞吐量、时延、丢包率)


# Latest version 最新版本

## Version: 4.3.0.230215
by itas109 on 2023-02-15

# Quick Start 快速开始

```
$ git clone --depth=1 https://github.com/itas109/CSerialPort.git
$ cd CSerialPort
$ mkdir bin && cd bin
$ cmake ..
$ cmake --build .
```

运行示例程序(如linux下串口环回测试)

```
CSerialPort/bin $ ./bin/CSerialPortDemoNoGui
Version: https://github.com/itas109/CSerialPort - V4.3.0.230215

availableFriendlyPorts:
1 - /dev/ttyS0

Please Input The Index Of Port(1 - 2)
1
Port Name: /dev/ttyS0
Open /dev/ttyS0 Success
Count: 1, Length: 29, Str: https://blog.csdn.net/itas109, Hex: 0x68 0x74 0x74 0x70 0x73 0x3A 0x2F 0x2F 0x62 0x6C 0x6F 0x67 0x2E 0x63 0x73 0x64 0x6E 0x2E 0x6E 0x65 0x74 0x2F 0x69 0x74 0x61 0x73 0x31 0x30 0x39
```

# Install CSerialPort Using Vcpkg 使用vcpkg安装CSerialPort

您可以通过[vcpkg](https://github.com/Microsoft/vcpkg/)依赖包管理工具下载和安装CSerialPort

```
$ git clone https://github.com/Microsoft/vcpkg.git
$ cd vcpkg
$ ./bootstrap-vcpkg.sh
$ ./vcpkg install cserialport
```

# Cross Compile 交叉编译

- arm on ubuntu 20.04

```
$ sudo apt-get install g++-arm-linux-gnueabi
$ cd CSerialPort
$ mkdir bin_arm && cd bin_arm
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=./cmake/toolchain_arm.cmake
$ cmake --build .
```

- aarch64 on ubuntu 20.04

```
$ sudo apt-get install g++-aarch64-linux-gnu
$ cd CSerialPort
$ mkdir bin_aarch64 && cd bin_aarch64
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=./cmake/toolchain_aarch64.cmake
$ cmake --build .
```

- mips64el on ubuntu 20.04

```
$ sudo apt-get install g++-mips64el-linux-gnuabi64
$ cd CSerialPort
$ mkdir bin_mips64el && cd bin_mips64el
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=./cmake/toolchain_mips64el.cmake
$ cmake --build .
```

- riscv64 on ubuntu 20.04

```
$ sudo apt-get install g++-riscv64-linux-gnu
$ cd CSerialPort
$ mkdir bin_riscv64 && cd bin_riscv64
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=./cmake/toolchain_riscv64.cmake
$ cmake --build .
```

# Screenshot 截图

## Gui 图形用户界面

示例路径: CSerialPort/examples/CommQT

![image](pic/linux.jpg)

## Tui 终端用户界面 

示例路径: CSerialPort/examples/CommTui

![image](pic/linux_tui.jpg)

## No Gui 无界面

示例路径: CSerialPort/examples/CommNoGui

![image](pic/linux_no_gui.jpg)

# Documents 文档

[API接口文档](doc/CSerialPort_doc_cn.chm)
[目录列表文档](doc/directory_list.md)
[错误指南文档](doc/error_guide.md)
[常见问题回答](doc/FAQ.md)

# Contacting 联系方式

- Email : itas109@qq.com
- QQ群 : [129518033](http://shang.qq.com/wpa/qunwpa?idkey=2888fa15c4513e6bfb9347052f36e437d919b2377161862948b2a49576679fc6)

# Links 链接

- [CSDN博客](https://blog.csdn.net/itas109)
- [Github](https://github.com/itas109/CSerialPort)
- [Gitee码云](https://gitee.com/itas109/CSerialPort)

# CSerialPort-based Applications 基于CSerialPort的应用
## 1. [CommMaster通信大师](https://gitee.com/itas109/CommMaster)

- 支持windows/linux/macos/raspberrypi等等
- 支持自定义串口名称
- 支持自定义波特率
- 支持自定义语言
- 支持自定义主题

https://gitee.com/itas109/CommMaster

![image](pic/CommMaster.png)

## 2. [CommLite](https://github.com/itas109/CommLite)

CommLite是一款基于CSerialPort的文本UI串口调试助手

- 支持x86, arm, mips等cpu架构
- 支持windows dos, linux, macos, raspberrypi, freebsd等操作系统

https://github.com/itas109/CommLite

![image](pic/commlite.gif)

# Donate 捐助

[CSDN博客](https://blog.csdn.net/itas109)

---

# Other branches 其他分支

仅支持windows版本分支 : https://github.com/itas109/CSerialPort/tree/CSerialPort_win_3.0.3

非常感谢[Remon Spekreijse的串口通信库](http://www.codeguru.com/cpp/i-n/network/serialcommunications/article.php/c2483/A-communication-class-for-serial-port.htm)

---

# License 开源协议

自 V3.0.0.171216 版本后采用[GNU Lesser General Public License v3.0](LICENSE)