/**
 * @file SerialPortInfoWinBase.h
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * QQ Group : 12951803
 * @brief the CSerialPortInfo windows class windows串口信息辅助类基类
 * @date 2020-04-29
 * @copyright The CSerialPort is Copyright (C) 2020 itas109. \n
 * Contact: itas109@qq.com \n\n
 *  You may use, distribute and copy the CSerialPort under the terms of \n
 *  GNU Lesser General Public License version 3, which is displayed below.
 */
#ifndef __CSERIALPORTINFOWINBASE_H__
#define __CSERIALPORTINFOWINBASE_H__

#include "SerialPortInfoBase.h"

/**
 * @brief the CSerialPortInfoBase windows class windows串口信息辅助类基类
 * @see inherit 继承 CSerialPortInfoBase
 * 
 */
class CSerialPortInfoWinBase :	public CSerialPortInfoBase
{
public:
	/**
	 * @brief Construct a new CSerialPortInfoWinBase object 构造函数
	 * 
	 */
	CSerialPortInfoWinBase();
	/**
	 * @brief Destroy the CSerialPortInfoWinBase object 析构函数
	 * 
	 */
	~CSerialPortInfoWinBase();

	/**
	* @brief available ports list 获取可用串口名称列表
	* @deprecated recommend use availablePortInfos 推荐使用availablePortInfos函数
	* @return return available port name list 返回可用串口名称列表 Windows:COM1 Linux:/dev/ttyS0
	*/
	static std::list<std::string> availablePorts();

    /**
     * @brief availableFriendlyPorts 获取可用串口友好名称列表
	 * @deprecated recommend use availablePortInfos 推荐使用availablePortInfos函数
     * @return return available port friendly name list 返回可用串口名称列表 Windows:COM1 Prolific USB-to-Serial Comm Port Linux:/dev/ttyS0(Not Support,so it is equal to availablePorts())
     */
    static std::list<std::string> availableFriendlyPorts();

	/**
     * @brief availablePortInfos 获取串口信息列表
     * @return return available port infolist 返回可用串口名称列表
     */
    static std::vector<SerialPortInfo> availablePortInfos();
};
#endif//__CSERIALPORTINFOWINBASE_H__
