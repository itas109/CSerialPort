/**
 * @file SerialPortNativeSyncWinBase.h
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief the CSerialPort Windows Base class windows串口基类
 * @copyright The CSerialPort is Copyright (C) 2014 itas109 <itas109@qq.com>. \n
 * You may use, copy, modify, and distribute the CSerialPort, under the terms \n
 * of the LICENSE file.
 */
#ifndef __CSERIALPORT_NATIVE_SYNC_WIN_BASE_H__
#define __CSERIALPORT_NATIVE_SYNC_WIN_BASE_H__

#include <tchar.h> //tchar

#include "SerialPortBase.h"

#include "windows.h"

/**
 * @brief the CSerialPort Windows Base class windows串口基类
 * @see inherit 继承 CSerialPortBase
 */
class CSerialPortNativeSyncWinBase : public CSerialPortBase
{
public:
    /**
     * @brief Construct a new CSerialPortNativeSyncWinBase object 构造函数
     *
     */
    CSerialPortNativeSyncWinBase();

    /**
     * @brief Construct a new CSerialPortNativeSyncWinBase object 通过串口名称构造函数
     *
     * @param portName  [in] the port name 串口名称 Windows:COM1 Linux:/dev/ttyS0
     */
    CSerialPortNativeSyncWinBase(const char *portName);

    /**
     * @brief Destroy the CSerialPortNativeSyncWinBase object 析构函数
     *
     */
    ~CSerialPortNativeSyncWinBase();

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
     * @brief if serial port is open success 串口是否打开成功
     *
     * @return
     * @retval true serial port open success 串口打开成功
     * @retval false serial port open failed 串口打开失败
     */
    bool isOpen() override final;

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
     * @brief read all data 读取所有数据
     *
     * @param data [out] read data result 读取结果
     * @return return number Of bytes read 返回读取字节数
     * @retval -1 read error 读取错误
     * @retval [other] return number Of bytes read 返回读取字节数
     */
    int readAllData(void *data) override final;

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
    HANDLE m_handle;

    COMMCONFIG m_comConfigure;
    COMMTIMEOUTS m_comTimeout;
};
#endif //__CSERIALPORT_NATIVE_SYNC_WIN_BASE_H__
