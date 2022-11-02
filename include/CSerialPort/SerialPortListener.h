/**
 * @file SerialPortListener.h
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief the CSerialPortListener interface class 串口事件监听接口类
 * @copyright The CSerialPort is Copyright (C) 2014 itas109 <itas109@qq.com>. \n
 * You may use, copy, modify, and distribute the CSerialPort, under the terms \n
 * of the LICENSE file.
 */
#ifndef __CSERIALPORT_LISTENER_H__
#define __CSERIALPORT_LISTENER_H__

namespace itas109
{
/**
 * @brief the CSerialPortListener class 串口事件监听类
 *
 */
class CSerialPortListener
{
public:
    /**
     * @brief Destroy the CSerialPortListener object 析构函数
     *
     */
    virtual ~CSerialPortListener() {}

    /**
     * @brief on read event 响应读取事件
     * @param portName [out] the port name 串口名称 Windows:COM1 Linux:/dev/ttyS0
     * @param readBufferLen [out] read buffer length 读取缓冲区数据长度
     */
    virtual void onReadEvent(const char *portName, unsigned int readBufferLen) = 0;
};
} // namespace itas109
#endif //__CSERIALPORT_LISTENER_H__
