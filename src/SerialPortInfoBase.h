/**
 * @file SerialPortInfoBase.h
 * @author itas109 (itas109@gmail.com) \n\n
 * Blog : http://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * QQ Group : 12951803
 * @brief the CSerialPortInfo Base class 串口信息辅助类基类
 * @date 2018-12-10
 * 
 * @copyright Copyright (c) 2019 - itas109
 * 
 */
#ifndef __CSERIALPORTINFOBASE_H__
#define __CSERIALPORTINFOBASE_H__

#include <list>

/**
 * @brief the CSerialPortInfo Base class 串口信息辅助类基类
 * 
 */
class CSerialPortInfoBase
{
public:
	/**
	 * @brief Construct a new CSerialPortInfoBase object 构造函数
	 * 
	 */
	CSerialPortInfoBase();
	/**
	 * @brief Destroy the CSerialPortInfoBase object 析构函数
	 * 
	 */
	~CSerialPortInfoBase();

	/**
	 * @brief available ports list 获取可用串口名称列表
	 *  
	 * @return return available port name list 返回可用串口名称列表 Windows:COM1 Linux:/dev/ttyS0
	 */
	static std::list<std::string> availablePorts();
};
#endif//__CSERIALPORTINFOBASE_H__
