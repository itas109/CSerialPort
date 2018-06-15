# CSerialPort

a lightweight library of serial port, which can easy to read and write serical port on windows with C++

# License
since V3.0.0.171216 use LGPL v3 License

---

Blog : http://blog.csdn.net/itas109

QQ Group : [12951803](http://shang.qq.com/wpa/qunwpa?idkey=2888fa15c4513e6bfb9347052f36e437d919b2377161862948b2a49576679fc6)

# Last Modify
## Version: 3.0.2.180615
by itas109 on 2018-06-15

http://blog.csdn.net/itas109

https://github.com/itas109
* 修复availablePorts函数不能枚举所有串口问题 fix function availablePorts can not enum all port error

---

## First Version by Remon Spekreijse on 2000-02-08
http://www.codeguru.com/cpp/i-n/network/serialcommunications/article.php/c2483/A-communication-class-for-serial-port.htm

## Second Version by mrlong on 2007-12-25
https://code.google.com/p/mycom/
* 增加 ClosePort
* 增加 WriteToPort 两个方法
* 增加 SendData 与 RecvData 方法


## by liquanhai on 2011-11-04
http://blog.csdn.net/liquanhai/article/details/4955253
* 增加 ClosePort 中交出控制权，防止死锁问题


## by liquanhai on 2011-11-06
http://blog.csdn.net/liquanhai/article/details/6941574
* 增加 ReceiveChar 中防止线程死锁


## by viruscamp on 2013-12-04
https://github.com/viruscamp/CSerialPort
* 增加 IsOpen 判断是否打开
* 修正 InitPort 中 parity Odd Even 参数取值错误
* 修改 InitPort 中 portnr 取值范围，portnr>9 时特殊处理
* 取消对 MFC 的依赖，使用 HWND 替代 CWnd，使用 win32 thread 函数而不是 MFC 的
* 增加用户消息编号自定义，方法来自 CnComm


## by itas109 on 2014-01-10
http://blog.csdn.net/itas109/article/details/18358297

https://github.com/itas109
* 解决COM10以上端口无法显示的问题 
* 扩展可选择端口，最大值MaxSerialPortNum可以自定义 
* 添加QueryKey()和Hkey2ComboBox两个方法，用于自动查询当前有效的串口号。


## by liquanhai on 2014-12-18
* 增加一些处理措施，主要是对减少CPU占用率

## by itas109 on 2016-05-07
http://blog.csdn.net/itas109
https://github.com/itas109
* 修复每次打开串口发送一次，当串口无应答时，需要关闭再打开或者接收完数据才能发送的问题。
  解决办法：在m_hEventArray中调整m_hWriteEvent的优先级高于读的优先级。CommThread(LPVOID pParam)函数中读写的位置也调换。
  参考：http://zhidao.baidu.com/link?url=RSrbPcfTZRULFFd2ziHZPBwnoXv1iCSu_Nmycb_yEw1mklT8gkoNZAkWpl3UDhk8L35DtRPo5VV5kEGpOx-Gea
* 修复停止位在头文件中定义成1导致SetCommState报错的问题，应为1对应的停止位是1.5。UINT stopsbits = ONESTOPBIT
* switch(stopbits)和switch(parity)增加默认情况，增强程序健壮性

## by itas109 on 2016-06-22
http://blog.csdn.net/itas109

https://github.com/itas109

* 增加ReceiveStr方法，用于接收字符串（接收缓冲区有多少字符就接收多少字符）。
       解决ReceiveChar只能接收单个字符的问题。

## by itas109 on 2016-06-29
http://blog.csdn.net/itas109

https://github.com/itas109
* 解决RestartMonitoring方法和StopMonitoring方法命令不准确引起的歧义，根据实际作用。
		将RestartMonitoring更改为ResumeMonitoring，将StopMonitoring更改为SuspendMonitoring。
* 增加IsThreadSuspend方法，用于判断线程是否挂起。
* 改进ClosePort方法，增加线程挂起判断，解决由于线程挂起导致串口关闭死锁的问题。
* 增加IsReceiveString宏定义，用于接收时采用单字节接收还是多字节接收

## by itas109 on 2016-08-02
http://blog.csdn.net/itas109

https://github.com/itas109
* 改进IsOpen方法，m_hComm增加INVALID_HANDLE_VALUE的情况，因为CreateFile方法失败返回的是INVALID_HANDLE_VALUE，不是NULL
* 改进ClosePort方法：增加串口句柄无效的判断(防止关闭死锁)；m_hWriteEvent不使用CloseHandle关闭
* 改进CommThread、ReceiveChar、ReceiveStr和WriteChar方法中异常处理的判断，增加三种判断：串口打开失败(error code:ERROR_INVALID_HANDLE)、连接过程中非法断开(error code:ERROR_BAD_COMMAND)和拒绝访问(error code:ERROR_ACCESS_DENIED)
* 采用安全函数sprintf_s和strcpy_s函数替换掉sprintf和strcpy
* 改进QueryKey方法，用于查询注册表的可用串口值，可以搜索到任意的可用串口
* 改进InitPort方法，串口打开失败，增加提示信息:串口不存在(error code:ERROR_FILE_NOT_FOUND)和串口拒绝访问(error code:ERROR_ACCESS_DENIED)
* 加入viruscamp 取消对 MFC 的依赖
* 改进InitPort方法，如果上次串口是打开，再次调用InitPort方法，关闭串口需要做一定的延时，否则有几率导致ERROR_ACCESS_DENIED拒绝访问，也就是串口占用问题
* 初始化默认波特率修改为9600
* 修复一些释放的BUG
* 规范了一些错误信息，参考winerror.h --  error code definitions for the Win32 API functions
* 删除SendData和RecvData方法

## by itas109 on 2016-08-10
http://blog.csdn.net/itas109

https://github.com/itas109
*  改进ReceiveStr方法，comstat.cbInQue = 0xcccccccc的情况（如串口异常断开），会导致RXBuff初始化失败

## by itas109 on 2017-02-14
http://blog.csdn.net/itas109

https://github.com/itas109
* 兼容ASCII和UNICODE编码
* ReceiveStr函数中发送函数SendMessage的第二个参数采用结构体形式，包括portNr串口号和bytesRead读取的字节数，可以处理16进制的时候0x00截断问题
* 精简不必要的函数SendData和RecvData
* 尽量的取消对 MFC 的依赖，Hkey2ComboBox函数暂时保留
* 其他小问题修改

## by itas109 on 2017-03-12
http://blog.csdn.net/itas109

https://github.com/itas109
* 增加宏定义_AFX，用于处理MFC的必要函数Hkey2ComboBox
* 进一步去除MFC依赖，修改AfxMessageBox函数

## by itas109 on 2017-12-16
## Version: 3.0.0.171216

http://blog.csdn.net/itas109

https://github.com/itas109
* 支持DLL输出
* 去除QueryKey和Hkey2ComboBox，采用CSerialPortInfo::availablePorts()函数代替
* 增加CSerialPortInfo类，目前只有availablePorts静态函数，用于获取活跃的串口到list
* 增加命名空间itas109
* 精简不必要的头文件
* InitPort和~CSerialPort()中直接整合ClosePort()

## by itas109 on 2018-02-14
## Version: 3.0.1.180214

http://blog.csdn.net/itas109

https://github.com/itas109
* ★修复不能连续发送的问题 ★ fix can not continue send error
* ★一次性写入尽可能多的数据到串口 ★ try best to send mutil data once in WriteChar funtion
* 修复BYTE内存设置的问题 fix BYTE memset error
* 在构造函数中初始化和释放临界区 initialize and delete critical section in Constructor
* 精简代码
---