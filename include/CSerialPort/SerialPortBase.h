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
#ifndef __CSERIALPORT_BASE_H__
#define __CSERIALPORT_BASE_H__

#include "SerialPort_global.h"

/**
 * @brief the CSerialPort Native Sync Base class 串口基类
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
                      unsigned int readBufferSize);

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
     * @brief get used length of buffer 获取读取缓冲区已使用大小
     *
     * @return return used length of buffer 返回读取缓冲区已使用大小
     */
    virtual unsigned int getReadBufferUsedLen() = 0;

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

public:
    /**
     * @brief read all data 读取所有数据
     *
     * @param data [out] read data result 读取结果
     * @return return number Of bytes read 返回读取字节数
     * @retval -1 read error 读取错误
     * @retval [other] return number Of bytes read 返回读取字节数
     */
    int readAllData(void *data);

    /**
     * @brief Set Debug Model 设置调试模式
     * @details output serial port read and write details info 输出串口读写的详细信息
     *
     * @param isDebug true if enable true为启用
     */
    void setDebugModel(bool isDebug);

    /**
     * @brief Get the Last Error object 获取最后的错误代码
     *
     * @return return last error code, refrence {@link itas109::SerialPortError} 错误代码
     */
    int getLastError() const;

    /**
     * @brief Get the Last Error Code Message 获取错误码信息
     *
     * @return return last error code message 返回错误码信息
     */
    const char *getLastErrorMsg() const;

    /**
     * @brief clear error 清除错误信息
     *
     */
    void clearError();

    /**
     * @brief Set the Port Name object 设置串口名称
     *
     * @param portName [in] the port name 串口名称 Windows:COM1 Linux:/dev/ttyS0
     */
    void setPortName(const char *portName);

    /**
     * @brief Get the Port Name object 获取串口名称
     *
     * @return return port name 返回串口名称
     */
    const char *getPortName() const;

    /**
     * @brief Set the Baud Rate object 设置波特率
     *
     * @param baudRate [in] the baudRate 波特率
     */
    void setBaudRate(int baudRate);

    /**
     * @brief Get the Baud Rate object 获取波特率
     *
     * @return return baudRate 返回波特率
     */
    int getBaudRate() const;

    /**
     * @brief Set the Parity object 设置校验位
     *
     * @param parity [in] the parity 校验位 {@link itas109::Parity}
     */
    void setParity(itas109::Parity parity);

    /**
     * @brief Get the Parity object 获取校验位
     *
     * @return return parity 返回校验位 {@link itas109::Parity}
     */
    itas109::Parity getParity() const;

    /**
     * @brief Set the Data Bits object 设置数据位
     *
     * @param dataBits [in] the dataBits 数据位 {@link itas109::DataBits}
     */
    void setDataBits(itas109::DataBits dataBits);

    /**
     * @brief Get the Data Bits object 获取数据位
     *
     * @return return dataBits 返回数据位 {@link itas109::DataBits}
     */
    itas109::DataBits getDataBits() const;

    /**
     * @brief Set the Stop Bits object 设置停止位
     *
     * @param stopbits [in] the stopbits 停止位 {@link itas109::StopBits}
     */
    void setStopBits(itas109::StopBits stopbits);

    /**
     * @brief Get the Stop Bits object 获取停止位
     *
     * @return return stopbits 返回停止位 {@link itas109::StopBits}
     */
    itas109::StopBits getStopBits() const;

    /**
     * @brief Set the Flow Control object 设置流控制
     *
     * @param flowControl [in]
     */
    void setFlowControl(itas109::FlowControl flowControl);

    /**
     * @brief Get the Flow Control object 获取流控制
     *
     * @return itas109::FlowControl
     */
    itas109::FlowControl getFlowControl() const;

    /**
     * @brief Set the Read Buffer Size object 设置读取缓冲区大小
     *
     * @param size [in] read buffer size 读取缓冲区大小
     */
    void setReadBufferSize(unsigned int size);

    /**
     * @brief Get the Read Buffer Size object 获取读取缓冲区大小
     *
     * @return return read buffer size 返回读取缓冲区大小
     */
    virtual unsigned int getReadBufferSize() const;

protected:
    int m_lastError;                    ///< last error code 最后的错误代码
    itas109::OperateMode m_operateMode; ///< operate mode 串口操作类型

    char m_portName[256];                    ///< port name 串口名称
    int m_baudRate;                          ///< badurate 波特率
    itas109::Parity m_parity;                ///< parity 校验位(0-4)
    itas109::DataBits m_dataBits;            ///< data bit 数据位(5-8)
    itas109::StopBits m_stopbits;            ///< stop bit 停止位(1 1.5 2)
    enum itas109::FlowControl m_flowControl; ///< flow control 流控(0-2)
    unsigned int m_readBufferSize;           ///< read buffer size 读取缓冲区大小

private:
};
#endif //__CSERIALPORT_BASE_H__
