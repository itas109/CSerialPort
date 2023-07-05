# [CSerialPort](https://github.com/itas109/CSerialPort)

[中文](README_zh_CN.md)

CSerialPort is a lightweight cross-platform serial port library based on C/C++, which can easy to read and write serial port on multiple operating system. Also support C#, Java, Python, Node.js etc.

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

# Design Principles

- Cross-platform
- Easy to use
- Higher efficiency

# Platform

CSerialPort was tested on the following platforms

- Windows ( x86, x86_64, arm64 )
- Linux ( x86, x86_64, arm, arm64/aarch64, mips64el, riscv, s390x, ppc64le )
- macOS ( x86_64 )
- Raspberry Pi ( armv7l )
- FreeBSD ( x86_64 )
- ...

# Todo List

## Long-term Goal

- [x] 1.support windows and linux first
- [ ] 2.add communicating protocol
- [x] 3.support hot swap - [CSerialPortExtend](https://github.com/itas109/CSerialPortExtend)
- [x] 4.higher efficiency notify module - replace with CSerialPortListener
- [x] 5.support other language - C#, Java, Python, Node.js etc. - more information [bindings](https://github.com/itas109/CSerialPort/tree/master/bindings)
- [x] 6.sync serial port communication
- [x] 7.new cross-platform gui serial port tool - [CommMaster](https://gitee.com/itas109/CommMaster)
- [x] 8.add introduction and tutorial of CSerialPort - [CSerialPort Tutorial ](https://blog.csdn.net/itas109/category_11520540.html)
- [ ] 9.serial port monitor hook

## Short-term Goal

- [x] 1.cross-platform OS identify class
- [x] 2.cross-platform thread class
- [x] 3.cross-platform lock class
- [x] 4.cross-platform higher efficiency timer class
- [ ] 5.cross-platform thread pool class
- [ ] 6.Performance test report(Throughput && delay && packet dropout rates)

# Latest version

## Version: 4.3.0.230215
by itas109 on 2023-02-15

# Quick Start

```
$ git clone --depth=1 https://github.com/itas109/CSerialPort.git
$ cd CSerialPort
$ mkdir bin && cd bin
$ cmake ..
$ cmake --build .
```

run demo ( for example serial port lookback test on linux)

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

# Install CSerialPort Using Vcpkg

You can download and install CSerialPort using the [vcpkg](https://github.com/Microsoft/vcpkg/) dependency manager

```
$ git clone https://github.com/Microsoft/vcpkg.git
$ cd vcpkg
$ ./bootstrap-vcpkg.sh
$ ./vcpkg install cserialport
```

# Cross Compile

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

# Screenshot

## Gui

Demo Path: CSerialPort/examples/CommQT

![image](pic/linux.jpg)

## Tui

Demo Path: CSerialPort/examples/CommTui

![image](pic/linux_tui.jpg)

## No Gui

Demo Path: CSerialPort/examples/CommNoGui

![image](pic/linux_no_gui.jpg)

# Documents

[API Document](doc/CSerialPort_doc_en.chm)
[Directory List Document](doc/directory_list.md)
[Error Guide Document](doc/error_guide.md)
[Frequently Asked Questions](doc/FAQ.md)

# Contacting

- Email : itas109@qq.com
- QQ Group : [129518033](http://shang.qq.com/wpa/qunwpa?idkey=2888fa15c4513e6bfb9347052f36e437d919b2377161862948b2a49576679fc6)

# Links

- [CSDN Blog](https://blog.csdn.net/itas109)
- [Github](https://github.com/itas109/CSerialPort)
- [Gitee](https://gitee.com/itas109/CSerialPort)

# CSerialPort-based Applications
## 1. [CommMaster](https://gitee.com/itas109/CommMaster)

- support windows/linux/macos/raspberrypi and so on
- support custom port name
- support custom baudrate
- support custom language
- support custom theme

https://gitee.com/itas109/CommMaster

![image](pic/CommMaster.png)

## 2. [CommLite](https://github.com/itas109/CommLite)

CommLite is tui's serial port tool based CSerialPort

- support x86, arm, mips cpu architecture
- support windows dos, linux, macos,raspberrypi, freebsd operating system

https://github.com/itas109/CommLite

![image](pic/commlite.gif)

# Donate

[CSDN Blog](https://blog.csdn.net/itas109)

---

# Other branches

Only for windows branch : https://github.com/itas109/CSerialPort/tree/CSerialPort_win_3.0.3

Thanks for [Remon Spekreijse's serial library](http://www.codeguru.com/cpp/i-n/network/serialcommunications/article.php/c2483/A-communication-class-for-serial-port.htm)

---

# License

since V3.0.0.171216 use [GNU Lesser General Public License v3.0](LICENSE)
