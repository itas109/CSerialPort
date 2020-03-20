/**
 * @file SerialPortInfoWinBase.h
 * @author itas109 (itas109@gmail.com) \n\n
 * Blog : http://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * QQ Group : 12951803
 * @brief the CSerialPortInfo windows class windows串口信息辅助类基类
 * @date 2018-12-10
 * 
 * @copyright Copyright (c) 2019 - itas109
 * 
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
	*
	* @return return available port name list 返回可用串口名称列表 Windows:COM1 Linux:/dev/ttyS0
	*/
	static std::list<std::string> availablePorts();

    /**
     * @brief availableFriendlyPorts 获取可用串口友好名称列表
     * @return return available port friendly name list 返回可用串口名称列表 Windows:COM1 Prolific USB-to-Serial Comm Port Linux:/dev/ttyS0(Not Support,so it is equal to availablePorts())
     */
    static std::list<std::string> availableFriendlyPorts();

private:
	/**
	 * @brief Get the Reg Key Values object 通过注册表获取串口名称列表
	 * 
	 * @param regKeyPath [in] the reg key path 注册表路径
	 * @param portsList [out] port name list 串口名称列表
	 * @return  
	 * @retval true true if excute success 执行成功返回true
	 * @retval false false if excute failed 执行失败返回false
	 */
	bool getRegKeyValues(std::string regKeyPath, std::list<std::string> & portsList);

    /**
     * @brief enumDetailsSerialPorts 通过setapi.lib枚举串口详细信息
     * @param portsList [out] port name list 串口友好名称列表
     * @return
     * @retval true true if excute success 执行成功返回true
     * @retval false false if excute failed 执行失败返回false
     */
    bool enumDetailsSerialPorts(std::list<std::string> &portsList);

private:
	static CSerialPortInfoWinBase* p_serialPortInfoWinBase;
};
#endif//__CSERIALPORTINFOWINBASE_H__
