/**
 * @file SerialPort.h
 * @author itas109 (itas109@gmail.com) \n\n
 * Blog : http://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * QQ Group : 12951803
 * @brief a lightweight library of serial port, which can easy to read and write serical port on windows and linux with C++ 轻量级跨平台串口读写类库
 * @version 4.0.0.181210
 * @date 2018-12-10
 * 
 * @copyright Copyright (c) 2019 - itas109
 * 
 */
#ifndef __CSERIALPORT_H__
#define __CSERIALPORT_H__

#include "SerialPort_global.h"
#include <string>

#include "sigslot.h"
using namespace sigslot;

class CSerialPortBase;

namespace itas109
{
	/**
	 * @brief the CSerialPort class 串口类库
	 * @see reference 引用 CSerialPortBase
	 */
	class DLL_EXPORT CSerialPort : public has_slots<>
	{
	public:
		/**
		 * @brief Construct a new CSerialPort object 构造函数
		 * 
		 */
		CSerialPort();
		/**
		 * @brief Construct a new CSerialPort object 通过串口名称构造函数
		 * 
		 * @param portName [in] the port name 串口名称 Windows:COM1 Linux:/dev/ttyS0
		 */
		CSerialPort(const std::string & portName);
		/**
		 * @brief Destroy the CSerialPort object 析构函数
		 * 
		 */
		~CSerialPort();

		/**
		 * @brief init 初始化函数
		 * 
		 * @param portName [in] the port name串口名称 Windows:COM1 Linux:/dev/ttyS0
		 * @param baudRate [in] the baudRate 波特率
		 * @param parity [in] the parity 校验位
		 * @param dataBits [in] the dataBits 数据位
		 * @param stopbits [in] the stopbits 停止位
		 * @param flowConctrol [in] flowConctrol type 流控制
		 * @param readBufferSize [in] the read buffer size 读取缓冲区大小
		 */
		void init(std::string portName, int baudRate = itas109::BaudRate9600, itas109::Parity parity = itas109::ParityNone, itas109::DataBits dataBits = itas109::DataBits8, itas109::StopBits stopbits = itas109::StopOne, itas109::FlowConctrol flowConctrol = itas109::FlowNone, int64 readBufferSize = 512);
		/**
		 * @brief  init 初始化函数
		 * 
		 * @param port [in] the port number 串口号
		 * @param baudRate [in] the baudRate 波特率
		 * @param parity [in] the parity 校验位
		 * @param dataBits [in] the dataBits 数据位
		 * @param stopbits [in] the stopbits 停止位
		 * @param flowConctrol [in] flowConctrol type 流控制
		 * @param readBufferSize [in] the read buffer size 读取缓冲区大小
		 */
		void init(int port, int baudRate = itas109::BaudRate9600, itas109::Parity parity = itas109::ParityNone, itas109::DataBits dataBits = itas109::DataBits8, itas109::StopBits stopbits = itas109::StopOne, itas109::FlowConctrol flowConctrol = itas109::FlowNone, int64 readBufferSize = 512);

		/**
		 * @brief open serial port 打开串口
		 * 
		 * @return 
		 * @retval true open success 打开成功
		 * @retval false open failed 打开失败
		 */
		bool open();
		/**
		 * @brief close 关闭串口
		 * 
		 */
		void close();

		/**
		 * @brief if serial port is open success 串口是否打开成功
		 * 
		 * @return 
		 * @retval true serial port open success 串口打开成功
		 * @retval false serial port open failed 串口打开失败
		 */
		bool isOpened();

		/**
		 * @brief read specified length data 读取指定长度数据
		 * 
		 * @param data [out] read data result 读取结果
		 * @param maxSize [in] read length 读取长度
		 * @return return number Of bytes read 返回读取字节数
		 * @retval -1 read error 读取错误
		 * @retval [other] return number Of bytes read 返回读取字节数
		 */
		int readData(char *data, int maxSize);
		/**
		 * @brief read all data 读取所有数据
		 * 
		 * @param data [out] read data result 读取结果
		 * @return return number Of bytes read 返回读取字节数
		 * @retval -1 read error 读取错误
		 * @retval [other] return number Of bytes read 返回读取字节数
		 */
		int readAllData(char *data);
		/**
		 * @brief read line data 读取一行字符串
		 * @todo Not implemented 未实现 
		 * 
		 * @param data 
		 * @param maxSize 
		 * @return int 
		 */
		int	readLineData(char *data, int maxSize);
		/**
		 * @brief write specified lenfth data 写入指定长度数据
		 * 
		 * @param data [in] write data 待写入数据
		 * @param maxSize [in] wtite length 写入长度
		 * @return return number Of bytes write 返回写入字节数
		 * @retval -1 read error 写入错误
		 * @retval [other] return number Of bytes write 返回写入字节数
		 */
		int writeData(const char * data, int maxSize);

		/**
		 * @brief Set Debug Model 设置调试模式
		 * @details output serial port read and write details info 输出串口读写的详细信息
		 * @todo  Not implemented 未实现 
		 * 
		 * @param isDebug true if enable true为启用
		 */
		void setDebugModel(bool isDebug);

		/**
		 * @brief Set the Read Time Interval object
		 * @details use timer import effectiveness 使用定时器提高效率
		 * @todo  Not implemented 未实现 
		 * 
		 * @param msecs read time micro second 读取间隔时间，单位：毫秒
		 */
		void setReadTimeInterval(int msecs);

		/**
		 * @brief Get the Last Error object 获取最后的错误代码
		 * 
		 * @return return last error code, refrence {@link itas109::SerialPortError} 错误代码
		 */
		 int getLastError() const;
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
		 void setPortName(std::string portName);
		 /**
		  * @brief Get the Port Name object 获取串口名称
		  * 
		  * @return return port name 返回串口名称
		  */
		 std::string getPortName() const;
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
		 * @param dataBits [in] the dataBits 数据位  {@link itas109::DataBits}
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
		 * @brief Set the Flow Conctrol object 设置流控制
		 * @todo Not implemented 未实现
		 * 
		 * @param flowConctrol [in]
		 */
		void setFlowConctrol(itas109::FlowConctrol flowConctrol);
		/**
		 * @brief Get the Flow Conctrol object 获取流控制
		 * @todo Not implemented 未实现
		 * 
		 * @return itas109::FlowConctrol 
		 */
		itas109::FlowConctrol getFlowConctrol() const;

		/**
		 * @brief Set the Read Buffer Size object
		 * 
		 * @param size [in] read buffer size  设置缓冲区大小
		 */
		void setReadBufferSize(int64 size);
		/**
		 * @brief Get the Read Buffer Size object 获取读取缓冲区大小
		 * 
		 * @return return read buffer size 返回读取缓冲区大小
		 */
		int64 getReadBufferSize() const;

		/**
		 * @brief Set the Dtr object 设置DTR
		 * @todo Not implemented 未实现
		 * 
		 * @param set [in]  
		 */
		void setDtr(bool set = true);
		/**
		 * @brief Set the Rts object 设置RTS
		 * @todo Not implemented 未实现
		 * 
		 * @param set [in]  
		 */
		void setRts(bool set = true);

		/**
		 * @brief Get the Version object 获取版本信息
		 * 
		 * @return return version 返回版本信息
		 */
		std::string getVersion();

	public:
		void onReadReady();
		sigslot::signal0<> readReady;//sigslot

	private:
		CSerialPortBase * p_serialPortBase;
	};
}
#endif //__CSERIALPORT_H__