/**
 * @file SerialPortUnixBase.h
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief the CSerialPort unix Base class unix串口基类
 * @copyright The CSerialPort is Copyright (C) 2014 itas109 <itas109@qq.com>. \n
 * You may use, copy, modify, and distribute the CSerialPort, under the terms \n
 * of the LICENSE file.
 */
#ifndef __CSERIALPORT_UNIX_BASE_H__
#define __CSERIALPORT_UNIX_BASE_H__

#include <stdio.h>     // Standard input/output definitions
#include <string.h>    // String function definitions
#include <sys/ioctl.h> // ioctl
#include <termios.h>   // POSIX terminal control definitions
#include <fcntl.h>     // File control definitions
#include <unistd.h>    // UNIX standard function definitions
#include <errno.h>     // Error number definitions

#include "SerialPortAsyncBase.h"

// Serial Programming Guide for POSIX Operating Systems
// https://www.msweet.org/serial/serial.html

/**
 * @brief the CSerialPort unix Base class unix串口基类
 * @see inherit 继承 CSerialPortAsyncBase
 *
 */
class CSerialPortUnixBase : public CSerialPortAsyncBase
{
public:
    /**
     * @brief Construct a new CSerialPortUnixBase object 构造函数
     *
     */
    CSerialPortUnixBase();
    /**
     * @brief Construct a new CSerialPortUnixBase object 通过串口名称构造函数
     *
     * @param portName [in] the port name 串口名称 Windows:COM1 Linux:/dev/ttyS0
     */
    CSerialPortUnixBase(const char *portName);
    /**
     * @brief Destroy the CSerialPortUnixBase object 析构函数
     *
     */
    ~CSerialPortUnixBase();

    /**
     * @brief open serial port 打开串口
     *
     * @return
     * @retval true open success 打开成功
     * @retval false open failed 打开失败
     */
    bool openPort() override final;
    /**
     * @brief close 关闭串口
     *
     */
    void closePort() override final;

    /**
     * @brief get used length of buffer 获取读取缓冲区已使用大小
     *
     * @return return used length of buffer 返回读取缓冲区已使用大小
     */
    unsigned int getReadBufferUsedLen() override final;

    /**
     * @brief read specified length data 读取指定长度数据
     *
     * @param data [out] read data result 读取结果
     * @param size [in] read length 读取长度
     * @return return number Of bytes read 返回读取字节数
     * @retval -1 read error 读取错误
     * @retval [other] return number Of bytes read 返回读取字节数
     */
    int readData(void *data, int size) override final;

    /**
     * @brief write specified lenfth data 写入指定长度数据
     *
     * @param data [in] write data 待写入数据
     * @param size [in] wtite length 写入长度
     * @return return number Of bytes write 返回写入字节数
     * @retval -1 read error 写入错误
     * @retval [other] return number Of bytes write 返回写入字节数
     */
    int writeData(const void *data, int size) override final;

    /**
     * @brief flush buffers after write 等待发送完成后刷新缓冲区
     *
     * @return
     * @retval true flush success 刷新缓冲区成功
     * @retval false flush failed 刷新缓冲区失败
     */
    bool flushBuffers() override final;

    /**
     * @brief flush read buffers 刷新接收缓冲区
     *
     * @return
     * @retval true flush success 刷新缓冲区成功
     * @retval false flush failed 刷新缓冲区失败
     */
    bool flushReadBuffers() override final;

    /**
     * @brief flush write buffers 刷新发送缓冲区
     *
     * @return
     * @retval true flush success 刷新缓冲区成功
     * @retval false flush failed 刷新缓冲区失败
     */
    bool flushWriteBuffers() override final;

    /**
     * @brief Set the Dtr object 设置DTR
     *
     * @param set [in]
     */
    void setDtr(bool set = true) override final;
    /**
     * @brief Set the Rts object 设置RTS
     *
     * @param set [in]
     */
    void setRts(bool set = true) override final;

private:
    /**
     * @brief rate2Constant baudrate to constant 波特率转为unix常量
     * @param baudrate 波特率
     * @return constant unix常量
     */
    int rate2Constant(int baudrate);

    /**
     * @brief uartSet
     * @param fd [in] file discriptor 文件描述符
     * @param baudRate [in] the baudRate 波特率
     * @param parity [in] the parity 校验位
     * @param dataBits [in] the dataBits 数据位
     * @param stopbits [in] the stopbits 停止位
     * @param flowControl [in] flowControl type 流控制
     * @return 0 success -1 error
     */
    int uartSet(int fd,
                int baudRate = itas109::BaudRate9600,
                itas109::Parity parity = itas109::ParityNone,
                itas109::DataBits dataBits = itas109::DataBits8,
                itas109::StopBits stopbits = itas109::StopOne,
                itas109::FlowControl flowControl = itas109::FlowNone);

    /**
     * @brief before stop read thread 停止读取多线程之前的操作
     *
     * @return void
     */
    void beforeStopReadThread() override final;

    /**
     * @brief read specified length data 读取指定长度数据
     *
     * @param data [out] read data result 读取结果
     * @param size [in] read length 读取长度
     * @return return number Of bytes read 返回读取字节数
     * @retval -1 read error 读取错误
     * @retval [other] return number Of bytes read 返回读取字节数
     */
    int readDataNative(void *data, int size) override final;

    /**
     * @brief get used length of native buffer 获取系统读缓冲区已使用大小
     *
     * @return return used length of native buffer 返回系统读缓冲区已使用大小
     */
    unsigned int getReadBufferUsedLenNative() override final;

    /**
     * @brief before stop read thread 停止读取多线程之前的操作
     *
     * @return
     * @retval 1 wait comm event success 等待串口事件成功
     * @retval 0 wait comm event timeout 等待串口事件超时
     * @retval -1 wait comm event failed 等待串口事件失败
     */
    int waitCommEventNative() override final;

private:
    int pipefd[2]; ///< pipe file descriptor for wake up read thread 用于唤醒读取线程的管道文件描述符
};
#endif //__CSERIALPORT_UNIX_BASE_H__
