/**
 * @file SerialPortWinBase.h
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
#ifndef __CSERIALPORTWINBASE_H__
#define __CSERIALPORTWINBASE_H__

#include <tchar.h> //tchar

#include "ithread.hpp"
#include "ibuffer.hpp"
#include "SerialPortBase.h"

/**
 * @brief the CSerialPort Windows Base class windows串口基类
 * @see inherit 继承 CSerialPortBase
 * @todo
 * 1. 多线程内部接管，无需开发者关心 \n
 * 2. 使用自动锁处理多线程互斥访问问题 \n
 * 3. 设计为虚函数，开发者可以继承修改 \n
 * 4. 支持热插拔 \n
 * 5. 支持通用通信协议:帧头 + 数据 + 校验 + 帧尾 \n
 * 6. 支持基于时间片的数据组装 \n
 * 7. 基于时间戳的数据缓存队列 - 数据存到缓冲区，并标记时间戳 开发者按照一定策略获取及组装数据 \n
 * 8. 重写windows下ReadFile、WriteFile \n
 * @warning More than one character can trigger read event 注意：只有超过一个字符才能出发接收事件
 */
class CSerialPortWinBase : public CSerialPortBase
{
public:
    /**
     * @brief Construct a new CSerialPortWinBase object 构造函数
     *
     */
    CSerialPortWinBase();
    /**
     * @brief Construct a new CSerialPortWinBase object 通过串口名称构造函数
     *
     * @param portName  [in] the port name 串口名称 Windows:COM1 Linux:/dev/ttyS0
     */
    CSerialPortWinBase(const char *portName);
    /**
     * @brief Destroy the CSerialPortWinBase object 析构函数
     *
     */
    virtual ~CSerialPortWinBase();

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
                      int baudRate = itas109::BaudRate9600,
                      itas109::Parity parity = itas109::ParityNone,
                      itas109::DataBits dataBits = itas109::DataBits8,
                      itas109::StopBits stopbits = itas109::StopOne,
                      itas109::FlowControl flowControl = itas109::FlowNone,
                      unsigned int readBufferSize = 4096);

    /**
     * @brief open serial port 打开串口
     *
     * @return
     * @retval true open success 打开成功
     * @retval false open failed 打开失败
     */
    virtual bool openPort();
    /**
     * @brief close 关闭串口
     *
     */
    virtual void closePort();

    /**
     * @brief if serial port is open success 串口是否打开成功
     *
     * @return
     * @retval true serial port open success 串口打开成功
     * @retval false serial port open failed 串口打开失败
     */
    virtual bool isOpen();

    /**
     * @brief get used length of buffer 获取读取缓冲区已使用大小
     *
     * @return return used length of buffer 返回读取缓冲区已使用大小
     */
    virtual unsigned int getReadBufferUsedLen() const;

    /**
     * @brief read specified length data 读取指定长度数据
     *
     * @param data [out] read data result 读取结果
     * @param size [in] read length 读取长度
     * @return return number Of bytes read 返回读取字节数
     * @retval -1 read error 读取错误
     * @retval [other] return number Of bytes read 返回读取字节数
     */
    virtual int readData(void *data, int size);
    /**
     * @brief read all data 读取所有数据
     *
     * @param data [out] read data result 读取结果
     * @return return number Of bytes read 返回读取字节数
     * @retval -1 read error 读取错误
     * @retval [other] return number Of bytes read 返回读取字节数
     */
    virtual int readAllData(void *data);
    /**
     * @brief read line data 读取一行字符串
     * @todo Not implemented 未实现
     *
     * @param data
     * @param size
     * @return int
     */
    virtual int readLineData(void *data, int size);
    /**
     * @brief write specified lenfth data 写入指定长度数据
     *
     * @param data [in] write data 待写入数据
     * @param size [in] wtite length 写入长度
     * @return return number Of bytes write 返回写入字节数
     * @retval -1 read error 写入错误
     * @retval [other] return number Of bytes write 返回写入字节数
     */
    virtual int writeData(const void *data, int size);

    /**
     * @brief Set Debug Model 设置调试模式
     * @details output serial port read and write details info 输出串口读写的详细信息
     * @todo  Not implemented 未实现
     *
     * @param isDebug true if enable true为启用
     */
    virtual void setDebugModel(bool isDebug);

    /**
     * @brief Set Read Interval Timeout millisecond
     * @details use timer import effectiveness 使用定时器提高效率
     *
     * @param msecs read time timeout millisecond 读取间隔时间，单位：毫秒
     */
    virtual void setReadIntervalTimeout(unsigned int msecs);

    /**
     * @brief setMinByteReadNotify set minimum byte of read notify 设置读取通知触发最小字节数
     * @param minByteReadNotify minimum byte of read notify 读取通知触发最小字节数
     */
    virtual void setMinByteReadNotify(unsigned int minByteReadNotify = 2);

    /**
     * @brief flush buffers after write 等待发送完成后刷新缓冲区
     *
     * @return
     * @retval true flush success 刷新缓冲区成功
     * @retval false flush failed 刷新缓冲区失败
     */
    virtual bool flushBuffers();

    /**
     * @brief flush read buffers 刷新接收缓冲区
     *
     * @return
     * @retval true flush success 刷新缓冲区成功
     * @retval false flush failed 刷新缓冲区失败
     */
    virtual bool flushReadBuffers();

    /**
     * @brief flush write buffers 刷新发送缓冲区
     *
     * @return
     * @retval true flush success 刷新缓冲区成功
     * @retval false flush failed 刷新缓冲区失败
     */
    virtual bool flushWriteBuffers();

    /**
     * @brief Set the Port Name object 设置串口名称
     *
     * @param portName [in] the port name 串口名称 Windows:COM1 Linux:/dev/ttyS0
     */
    virtual void setPortName(const char *portName);
    /**
     * @brief Get the Port Name object 获取串口名称
     *
     * @return return port name 返回串口名称
     */
    virtual const char *getPortName() const;

    /**
     * @brief Set the Baud Rate object 设置波特率
     *
     * @param baudRate [in] the baudRate 波特率
     */
    virtual void setBaudRate(int baudRate);
    /**
     * @brief Get the Baud Rate object 获取波特率
     *
     * @return return baudRate 返回波特率
     */
    virtual int getBaudRate() const;

    /**
     * @brief Set the Parity object 设置校验位
     *
     * @param parity [in] the parity 校验位 {@link itas109::Parity}
     */
    virtual void setParity(itas109::Parity parity);
    /**
     * @brief Get the Parity object 获取校验位
     *
     * @return return parity 返回校验位 {@link itas109::Parity}
     */
    virtual itas109::Parity getParity() const;
    /**
     * @brief Set the Data Bits object 设置数据位
     *
     * @param dataBits [in] the dataBits 数据位 {@link itas109::DataBits}
     */
    virtual void setDataBits(itas109::DataBits dataBits);
    /**
     * @brief Get the Data Bits object 获取数据位
     *
     * @return return dataBits 返回数据位 {@link itas109::DataBits}
     */
    virtual itas109::DataBits getDataBits() const;

    /**
     * @brief Set the Stop Bits object 设置停止位
     *
     * @param stopbits [in] the stopbits 停止位 {@link itas109::StopBits}
     */
    virtual void setStopBits(itas109::StopBits stopbits);
    /**
     * @brief Get the Stop Bits object 获取停止位
     *
     * @return return stopbits 返回停止位 {@link itas109::StopBits}
     */
    virtual itas109::StopBits getStopBits() const;

    /**
     * @brief Set the Flow Control object 设置流控制
     * @todo Not implemented 未实现
     *
     * @param flowControl [in]
     */
    virtual void setFlowControl(itas109::FlowControl flowControl);
    /**
     * @brief Get the Flow Control object 获取流控制
     * @todo Not implemented 未实现
     *
     * @return itas109::FlowControl
     */
    virtual itas109::FlowControl getFlowControl() const;

    /**
     * @brief Set the Read Buffer Size object 设置读取缓冲区大小
     *
     * @param size [in] read buffer size 读取缓冲区大小
     */
    virtual void setReadBufferSize(unsigned int size);
    /**
     * @brief Get the Read Buffer Size object 获取读取缓冲区大小
     *
     * @return return read buffer size 返回读取缓冲区大小
     */
    virtual unsigned int getReadBufferSize() const;

    /**
     * @brief Set the Dtr object 设置DTR
     *
     * @param set [in]
     */
    virtual void setDtr(bool set = true);
    /**
     * @brief Set the Rts object 设置RTS
     *
     * @param set [in]
     */
    virtual void setRts(bool set = true);

public:
    /**
     * @brief Get the Overlap Monitor object 获取监视器的overlapped
     *
     * @return return monitor overlapped 返回监视器的overlapped
     */
    OVERLAPPED getOverlapMonitor();
    /**
     * @brief Get the Main Handle object 获取handle
     *
     * @return retrun handle 返回handle
     */
    HANDLE getMainHandle();

    /**
     * @brief isThreadRunning 是否启动多线程
     * @return
     * @retval true thread running 多线程已启动
     * @retval false thread not running 多线程未启动
     */
    bool isThreadRunning();

private:
    /**
     * @brief thread monitor 多线程监视器
     *
     */
    static unsigned int __stdcall commThreadMonitor(LPVOID pParam);
    /**
     * @brief start thread monitor 启动多线程监视器
     *
     * @return
     * @retval true start success 启动成功
     * @retval false start failed 启动失败
     */
    bool startThreadMonitor();
    /**
     * @brief stop thread monitor 停止多线程监视器
     *
     * @return
     * @retval true stop success 停止成功
     * @retval false stop failed 停止失败
     */
    bool stopThreadMonitor();

    /**
     * @brief read specified length data 读取指定长度数据
     *
     * @param data [out] read data result 读取结果
     * @param size [in] read length 读取长度
     * @return return number Of bytes read 返回读取字节数
     * @retval -1 read error 读取错误
     * @retval [other] return number Of bytes read 返回读取字节数
     */
    virtual int readDataWin(void *data, int size);

private:
    char m_portName[256];
    int m_baudRate;
    itas109::Parity m_parity;
    itas109::DataBits m_dataBits;
    itas109::StopBits m_stopbits;
    enum itas109::FlowControl m_flowControl;
    unsigned int m_readBufferSize;

private:
    HANDLE m_handle;

    itas109::i_thread_t m_monitorThread;
    OVERLAPPED overlapMonitor; ///< monitor overlapped

    OVERLAPPED m_overlapRead;  ///< read overlapped
    OVERLAPPED m_overlapWrite; ///< write overlapped

    COMMCONFIG m_comConfigure;
    COMMTIMEOUTS m_comTimeout;

    CRITICAL_SECTION m_communicationMutex; ///< mutex

    bool m_isThreadRunning;

    itas109::RingBuffer<char> *p_buffer; ///< receive buffer
};
#endif //__CSERIALPORTWINBASE_H__
