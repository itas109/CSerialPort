/**
 * @file SerialPortAsyncBase.h
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief the CSerialPort Async Base class 异步串口基类
 * @copyright The CSerialPort is Copyright (C) 2014 itas109 <itas109@qq.com>. \n
 * You may use, copy, modify, and distribute the CSerialPort, under the terms \n
 * of the LICENSE file.
 */
#ifndef __CSERIALPORT_ASYNC_BASE_H__
#define __CSERIALPORT_ASYNC_BASE_H__

#include "SerialPortBase.h"
#include "ithread.hpp"

namespace itas109
{
// class IMutex;
class CSerialPortListener;
class CSerialPortHotPlugListener;
class CSerialPortHotPlug;
template <class T>
class ITimer;
class IProtocolParser;
} // namespace itas109

/**
 * @brief the CSerialPort Async Base class 异步串口基类
 *
 */
class CSerialPortAsyncBase: public CSerialPortBase
{
public:
    /**
     * @brief Construct a new CSerialPortAsyncBase object 构造函数
     *
     */
    CSerialPortAsyncBase();
    /**
     * @brief Construct a new CSerialPortAsyncBase object 通过串口名称构造函数
     *
     * @param portName [in] the port name 串口名称 Windows:COM1 Linux:/dev/ttyS0
     */
    CSerialPortAsyncBase(const char *portName);
    /**
     * @brief Destroy the CSerialPortAsyncBase object 析构函数
     *
     */
    virtual ~CSerialPortAsyncBase();

    /**
     * @brief connect read event 连接读取事件
     *
     * @param event [in] serial port listener 串口监听事件类
     * @return return connect status 返回连接状态
     * @retval 0 success 成功
     * @retval 14 invalid parameter error 无效的参数
     */
    int connectReadEvent(itas109::CSerialPortListener *event);

    /**
     * @brief disconnect read event 断开连接读取事件
     *
     * @return return disconnect status 返回断开连接状态
     * @retval 0 success 成功
     * @retval [other] failed 失败
     */
    int disconnectReadEvent();

    /**
     * @brief connect hot plug event 连接串口热插拔事件
     *
     * @param event [in] serial port hot plug listener 串口热插拔事件类
     * @return return connect status 返回连接状态
     * @retval 0 success 成功
     * @retval 14 invalid parameter error 无效的参数
     */
    int connectHotPlugEvent(itas109::CSerialPortHotPlugListener *event);

    /**
     * @brief disconnect hot plug event 断开串口热插拔事件
     *
     * @return return disconnect status 返回断开串口热插拔状态
     * @retval 0 success 成功
     * @retval [other] failed 失败
     */
    int disconnectHotPlugReadEvent();

    /**
     * @brief set protocol parser 设置协议解析器
     *
     * @param parser [in] protocol parser 协议解析器
     * @return return set status 返回设置状态
     * @retval 0 success 成功
     * @retval 14 invalid parameter error 无效的参数
     */
    int setProtocolParser(itas109::IProtocolParser *parser);

    /**
     * @brief get used length of buffer 获取读取缓冲区已使用大小
     *
     * @return return used length of buffer 返回读取缓冲区已使用大小
     */
    virtual unsigned int getReadBufferUsedLen() = 0;

    /**
     * @brief Set Read Interval Timeout millisecond
     * @details use timer import effectiveness 使用定时器提高效率
     *
     * @param msecs read time timeout millisecond 读取间隔时间，单位：毫秒
     */
    virtual void setReadIntervalTimeout(unsigned int msecs) = 0;

    /**
     * @brief Get Read Interval Timeout millisecond
     *
     * @return read time timeout millisecond 读取间隔时间，单位：毫秒
     */
    virtual unsigned int getReadIntervalTimeout();

    /**
     * @brief setMinByteReadNotify set minimum byte of read notify 设置读取通知触发最小字节数
     * @param minByteReadNotify minimum byte of read notify 读取通知触发最小字节数
     */
    virtual void setMinByteReadNotify(unsigned int minByteReadNotify) = 0;

    /**
     * @brief getMinByteReadNotify get minimum byte of read notify 获取读取通知触发最小字节数
     * @return minimum byte of read notify 读取通知触发最小字节数
     */
    virtual unsigned int getMinByteReadNotify();

    /**
     * @brief setByteReadBufferFullNotify set byte of read buffer full notify 设置读取通知触发缓冲区字节数
     * @param byteReadBufferFullNotify byte of read buffer full notify 读取通知触发缓冲区字节数
     */
    virtual void setByteReadBufferFullNotify(unsigned int byteReadBufferFullNotify);

    /**
     * @brief getByteReadBufferFullNotify get byte of read buffer full notify 获取读取通知触发缓冲区字节数
     * @return byte of read buffer full notify 读取通知触发缓冲区字节数
     */
    virtual unsigned int getByteReadBufferFullNotify();

protected:
    unsigned int m_readIntervalTimeoutMS;                   ///< read time timeout millisecond 读取间隔时间，单位：毫秒
    unsigned int m_minByteReadNotify;                       ///< minimum byte of read notify 读取通知触发最小字节数
    unsigned int m_byteReadBufferFullNotify;                ///< byte of read buffer full notify 读取通知触发缓冲区字节数
    itas109::IMutex m_mutex;                                ///< mutex 互斥锁
    itas109::IMutex m_mutexRead;                            ///< read mutex 读互斥锁
    itas109::IMutex m_mutexWrite;                           ///< write mutex 写互斥锁
    itas109::CSerialPortListener *p_readEvent;              ///< read event 读取事件
    itas109::ITimer<itas109::CSerialPortListener> *p_timer; ///< read timer 读取定时器
    itas109::CSerialPortHotPlug *p_serialPortHotPlug;       ///< serial port hot plug class 串口热插拔类
    itas109::IProtocolParser *p_protocolParser;             ///< protocol parse 通信协议解析

private:
};
#endif //__CSERIALPORT_ASYNC_BASE_H__
