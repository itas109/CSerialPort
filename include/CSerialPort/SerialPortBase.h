/**
 * @file SerialPortBase.h
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief the CSerialPort Base class 串口基类
 * @copyright The CSerialPort is Copyright (C) 2014 itas109 <itas109@qq.com>. \n
 * You may use, copy, modify, and distribute the CSerialPort, under the terms \n
 * of the LICENSE file.
 */
#ifndef __CSERIALPORTBASE_H__
#define __CSERIALPORTBASE_H__

#include "SerialPort_global.h"

namespace itas109
{
class IMutex;
class CSerialPortListener;
template <class T> class ITimer;
} // namespace itas109

/**
 * @brief the CSerialPort Base class 串口基类
 *
 */
class CSerialPortBase
{
public:
    /**
     * @brief Construct a new CSerialPortBase object 构造函数
     *
     */
    CSerialPortBase();
    /**
     * @brief Construct a new CSerialPortBase object 通过串口名称构造函数
     *
     * @param portName [in] the port name 串口名称 Windows:COM1 Linux:/dev/ttyS0
     */
    CSerialPortBase(const char *portName);
    /**
     * @brief Destroy the CSerialPortBase object 析构函数
     *
     */
    virtual ~CSerialPortBase();

    /**
     * @brief init 初始化函数
     *
     * @param portName [in] the port name串口名称 Windows:COM1 Linux:/dev/ttyS0
     * @param baudRate [in] the baudRate 波特率
     * @param parity [in] the parity 校验位
     * @param dataBits [in] the dataBits 数据位
     * @param stopbits [in] the stopbits 停止位
     * @param flowControl [in] flowControl type 流控制
     * @param readBufferSize [in] the read buffer size 读取缓冲区大小
     */
    virtual void init(const char *portName,
                      int baudRate,
                      itas109::Parity parity,
                      itas109::DataBits dataBits,
                      itas109::StopBits stopbits,
                      itas109::FlowControl flowControl,
                      unsigned int readBufferSize) = 0;

    /**
     * @brief Set the Operate Mode object 设置串口操作模式
     *
     * @param operateMode [in] the operate mode 串口操作模式 {@link itas109::OperateMode}
     */
    virtual void setOperateMode(itas109::OperateMode operateMode);

    /**
     * @brief open serial port 打开串口
     *
     * @return
     * @retval true open success 打开成功
     * @retval false open failed 打开失败
     */
    virtual bool openPort() = 0;
    /**
     * @brief close 关闭串口
     *
     */
    virtual void closePort() = 0;

    /**
     * @brief if serial port is open success 串口是否打开成功
     *
     * @return
     * @retval true serial port open success 串口打开成功
     * @retval false serial port open failed 串口打开失败
     */
    virtual bool isOpen() = 0;

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
     * @brief get used length of buffer 获取读取缓冲区已使用大小
     *
     * @return return used length of buffer 返回读取缓冲区已使用大小
     */
    virtual unsigned int getReadBufferUsedLen() const = 0;

    /**
     * @brief read specified length data 读取指定长度数据
     *
     * @param data [out] read data result 读取结果
     * @param size [in] read length 读取长度
     * @return return number Of bytes read 返回读取字节数
     * @retval -1 read error 读取错误
     * @retval [other] return number Of bytes read 返回读取字节数
     */
    virtual int readData(void *data, int size) = 0;
    /**
     * @brief read all data 读取所有数据
     *
     * @param data [out] read data result 读取结果
     * @return return number Of bytes read 返回读取字节数
     * @retval -1 read error 读取错误
     * @retval [other] return number Of bytes read 返回读取字节数
     */
    virtual int readAllData(void *data) = 0;
    /**
     * @brief read line data 读取一行字符串
     * @todo Not implemented 未实现
     *
     * @param data
     * @param size
     * @return int
     */
    virtual int readLineData(void *data, int size) = 0;
    /**
     * @brief write specified lenfth data 写入指定长度数据
     *
     * @param data [in] write data 待写入数据
     * @param size [in] wtite length 写入长度
     * @return return number Of bytes write 返回写入字节数
     * @retval -1 read error 写入错误
     * @retval [other] return number Of bytes write 返回写入字节数
     */
    virtual int writeData(const void *data, int size) = 0;

    /**
     * @brief Set Debug Model 设置调试模式
     * @details output serial port read and write details info 输出串口读写的详细信息
     *
     * @param isDebug true if enable true为启用
     */
    virtual void setDebugModel(bool isDebug) = 0;

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
     * @brief flush buffers after write 等待发送完成后刷新缓冲区
     *
     * @return
     * @retval true flush success 刷新缓冲区成功
     * @retval false flush failed 刷新缓冲区失败
     */
    virtual bool flushBuffers() = 0;

    /**
     * @brief flush read buffers 刷新接收缓冲区
     *
     * @return
     * @retval true flush success 刷新缓冲区成功
     * @retval false flush failed 刷新缓冲区失败
     */
    virtual bool flushReadBuffers() = 0;

    /**
     * @brief flush write buffers 刷新发送缓冲区
     *
     * @return
     * @retval true flush success 刷新缓冲区成功
     * @retval false flush failed 刷新缓冲区失败
     */
    virtual bool flushWriteBuffers() = 0;

    /**
     * @brief Get the Last Error object 获取最后的错误代码
     *
     * @return return last error code, refrence {@link itas109::SerialPortError} 错误代码
     */
    virtual int getLastError() const;
    /**
     * @brief Get the Last Error Code Message 获取错误码信息
     *
     * @return return last error code message 返回错误码信息
     */
    virtual const char *getLastErrorMsg() const;
    /**
     * @brief clear error 清除错误信息
     *
     */
    virtual void clearError();

    /**
     * @brief Set the Port Name object 设置串口名称
     *
     * @param portName [in] the port name 串口名称 Windows:COM1 Linux:/dev/ttyS0
     */
    virtual void setPortName(const char *portName) = 0;
    /**
     * @brief Get the Port Name object 获取串口名称
     *
     * @return return port name 返回串口名称
     */
    virtual const char *getPortName() const = 0;

    /**
     * @brief Set the Baud Rate object 设置波特率
     *
     * @param baudRate [in] the baudRate 波特率
     */
    virtual void setBaudRate(int baudRate) = 0;
    /**
     * @brief Get the Baud Rate object 获取波特率
     *
     * @return return baudRate 返回波特率
     */
    virtual int getBaudRate() const = 0;

    /**
     * @brief Set the Parity object 设置校验位
     *
     * @param parity [in] the parity 校验位 {@link itas109::Parity}
     */
    virtual void setParity(itas109::Parity parity) = 0;
    /**
     * @brief Get the Parity object 获取校验位
     *
     * @return return parity 返回校验位 {@link itas109::Parity}
     */
    virtual itas109::Parity getParity() const = 0;

    /**
     * @brief Set the Data Bits object 设置数据位
     *
     * @param dataBits [in] the dataBits 数据位 {@link itas109::DataBits}
     */
    virtual void setDataBits(itas109::DataBits dataBits) = 0;
    /**
     * @brief Get the Data Bits object 获取数据位
     *
     * @return return dataBits 返回数据位 {@link itas109::DataBits}
     */
    virtual itas109::DataBits getDataBits() const = 0;

    /**
     * @brief Set the Stop Bits object 设置停止位
     *
     * @param stopbits [in] the stopbits 停止位 {@link itas109::StopBits}
     */
    virtual void setStopBits(itas109::StopBits stopbits) = 0;
    /**
     * @brief Get the Stop Bits object 获取停止位
     *
     * @return return stopbits 返回停止位 {@link itas109::StopBits}
     */
    virtual itas109::StopBits getStopBits() const = 0;

    /**
     * @brief Set the Flow Control object 设置流控制
     *
     * @param flowControl [in]
     */
    virtual void setFlowControl(itas109::FlowControl flowControl) = 0;
    /**
     * @brief Get the Flow Control object 获取流控制
     *
     * @return itas109::FlowControl
     */
    virtual itas109::FlowControl getFlowControl() const = 0;

    /**
     * @brief Set the Read Buffer Size object 设置读取缓冲区大小
     *
     * @param size [in] read buffer size 读取缓冲区大小
     */
    virtual void setReadBufferSize(unsigned int size) = 0;
    /**
     * @brief Get the Read Buffer Size object 获取读取缓冲区大小
     *
     * @return return read buffer size 返回读取缓冲区大小
     */
    virtual unsigned int getReadBufferSize() const = 0;

    /**
     * @brief Set the Dtr object 设置DTR
     *
     * @param set [in]
     */
    virtual void setDtr(bool set = true) = 0;
    /**
     * @brief Set the Rts object 设置RTS
     *
     * @param set [in]
     */
    virtual void setRts(bool set = true) = 0;

protected:
    int m_lastError;                                        ///< last error code 最后的错误代码
    itas109::OperateMode m_operateMode;                     ///< operate mode 串口操作类型
    unsigned int m_readIntervalTimeoutMS;                   ///< read time timeout millisecond 读取间隔时间，单位：毫秒
    unsigned int m_minByteReadNotify;                       ///< minimum byte of read notify 读取通知触发最小字节数
    itas109::IMutex *p_mutex;                               ///< mutex 互斥锁
    itas109::CSerialPortListener *p_readEvent;              ///< read event 读取事件
    itas109::ITimer<itas109::CSerialPortListener> *p_timer; ///< read timer 读取定时器
private:
};
#endif //__CSERIALPORTBASE_H__
