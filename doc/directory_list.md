update : 2022-10-01

```
CSerialPort # root
+--- .clang-format # code format 代码规范
+--- CHANGELOG.md # change log 修改日志
+--- cmake # cross compile toolchain 交叉编译文件
|   +--- toolchain_aarch64.cmake
|   +--- toolchain_arm.cmake
|   +--- toolchain_mips64el.cmake
|   +--- toolchain_riscv64.cmake
+--- CMakeLists.txt
+--- LICENSE # LGPL3.0 license
+--- pic # picture 图片
+--- README-EN.md
+--- README.md
+--- VERSION # version 版本号
+--- cserialport-config.cmake.in
+--- cserialport.cppcheck
+--- doc # document 文档目录
|   +--- CSerialPort_doc_cn.chm # Chinese documnet 简体中文说明书
|   +--- CSerialPort_doc_en.chm # English documnet 英文说明书
|   +--- directory_list.md # directory list 目录列表
|   +--- Doxyfile.in # Doxyfile Doxy文档配置文件
|   +--- error_guide.md # error guide 错误指南文档
|   +--- FAQ.md # Frequently Asked Questions 常见问题回答
|   +--- How To Use.txt
|   +--- suspending.txt
+--- examples # example 示例目录
|   +--- CommMFC # CSerialPort MFC Demo use source code windows直接调用源码MFC程序示例
|   +--- CommMFCDLL # CSerialPort MFC Demo use dll windows调用动态库MFC程序示例
|   +--- CommNoGui # CSerialPort No Gui Demo 无界面程序示例
|   +--- CommNoGuiTimeout # CSerialPort No Gui Timeout Demo 无界面读取超时程序示例
|   +--- CommQT # CSerialPort QT Demo QT程序示例
|   +--- CommTui # CSerialPort tui Demo 文本界面程序示例
+--- include # headers 头文件
|   +--- CSerialPort
|   |   +--- ibuffer.hpp # lightweight cross-platform buffer library 轻量级跨平台缓冲区类
|   |   +--- ithread.hpp # lightweight cross-platform thread library 轻量级跨平台线程类
|   |   +--- itimer.hpp # lightweight cross-platform timer library 轻量级跨平台定时器类
|   |   +--- osplatformutil.h # os platform define 操作系统定义
|   |   +--- SerialPort.h # lightweight cross-platform serial port library 轻量级跨平台串口类库
|   |   +--- SerialPortBase.h # CSerialPort Base class 串口基类
|   |   +--- SerialPortInfo.h # CSerialPortInfo class 串口信息辅助类
|   |   +--- SerialPortInfoBase.h # CSerialPortInfo Base class 串口信息辅助类基类
|   |   +--- SerialPortInfoUnixBase.h # CSerialPortInfo unix class unix串口信息辅助类基类
|   |   +--- SerialPortInfoWinBase.h # CSerialPortInfo windows class windows串口信息辅助类基类
|   |   +--- SerialPortUnixBase.h # CSerialPort unix Base class unix串口基类
|   |   +--- SerialPortWinBase.h # CSerialPort Windows Base class windows串口基类
|   |   +--- SerialPort_global.h # Global difine of CSerialPort 串口全局定义 
|   |   +--- SerialPort_version.h # CSerialPort version 版本定义
|   |   +--- sigslot.h # signal and slot 信号与槽
+--- lib # lib 库目录
|   +--- CMakeLists.txt # CSerialPort library cmake file [recommend]
|   +--- Linux # linux lib linux库目录
|   +--- Windows # windows lib windows库目录
+--- src # source 源代码
+--- test # unit test 单元测试
```
