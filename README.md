CSerialPort
===========

First Version by Remon Spekreijse on 2000-02-08
http://www.codeguru.com/cpp/i-n/network/serialcommunications/article.php/c2483/A-communication-class-for-serial-port.htm


Second Version by mrlong on 2007-12-25
https://code.google.com/p/mycom/
* 增加 ClosePort
* 增加 WriteToPort 两个方法
* 增加 SendData 与 RecvData 方法


by liquanhai on 2011-11-04
http://blog.csdn.net/liquanhai/article/details/4955253
* 增加 ClosePort 中交出控制权，防止死锁问题


by liquanhai on 2011-11-06
http://blog.csdn.net/liquanhai/article/details/6941574
* 增加 ReceiveChar 中防止线程死锁


by viruscamp on 2013-12-04
https://github.com/viruscamp/CSerialPort
* 增加 IsOpen 判断是否打开
* 修正 InitPort 中 parity Odd Even 参数取值错误
* 修改 InitPort 中 portnr 取值范围，portnr>9 时特殊处理
* 取消对 MFC 的依赖，使用 HWND 替代 CWnd，使用 win32 thread 函数而不是 MFC 的
* 增加用户消息编号自定义，方法来自 CnComm


by itas109 on 2014-01-10
http://blog.csdn.net/itas109/article/details/18358297
* 解决COM10以上端口无法显示的问题 
* 扩展可选择端口，最大值MaxSerialPortNum可以自定义 
* 添加QueryKey()和Hkey2ComboBox两个方法，用于自动查询当前有效的串口号。


by liquanhai on 2014-12-18
* 增加一些处理措施，主要是对减少CPU占用率

by itas109 on 2016-05-07
http://blog.csdn.net/itas109
* 修复每次打开串口发送一次，当串口无应答时，需要关闭再打开或者接收完数据才能发送的问题。
  解决办法：在m_hEventArray中调整m_hWriteEvent的优先级高于读的优先级。CommThread(LPVOID pParam)函数中读写的位置也调换。
  参考：http://zhidao.baidu.com/link?url=RSrbPcfTZRULFFd2ziHZPBwnoXv1iCSu_Nmycb_yEw1mklT8gkoNZAkWpl3UDhk8L35DtRPo5VV5kEGpOx-Gea
* 修复停止位在头文件中定义成1导致SetCommState报错的问题，应为1对应的停止位是1.5。UINT stopsbits = ONESTOPBIT
* switch(stopbits)和switch(parity)增加默认情况，增强程序健壮性

by itas109 on 2016-06-22
http://blog.csdn.net/itas109
* 增加ReceiveStr方法，用于接收字符串（接收缓冲区有多少字符就接收多少字符）。
       解决ReceiveChar只能接收单个字符的问题。

by itas109 on 2016-06-29
http://blog.csdn.net/itas109
* 解决RestartMonitoring方法和StopMonitoring方法命令不准确引起的歧义，根据实际作用。
		将RestartMonitoring更改为ResumeMonitoring，将StopMonitoring更改为SuspendMonitoring。
* 增加IsThreadSuspend方法，用于判断线程是否挂起。
* 改进ClosePort方法，增加线程挂起判断，解决由于线程挂起导致串口关闭死锁的问题。
* 增加IsReceiveString宏定义，用于接收时采用单字节接收还是多字节接收

