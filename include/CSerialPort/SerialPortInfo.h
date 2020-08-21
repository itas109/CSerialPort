/**
 * @file SerialPortInfo.h
 * @author itas109 (itas109@gmail.com) \n\n
 * Blog : http://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * QQ Group : 12951803
 * @brief the CSerialPortInfo class 串口信息辅助类
 * @date 2018-12-10
 *
 * @copyright Copyright (c) 2019 - itas109
 *
 */
#ifndef __CSERIALPORTINFO_H__
#define __CSERIALPORTINFO_H__

#include "SerialPort_global.h"
#include "SerialPortInfoBase.h"

#include <string>
#include <list>

namespace itas109
{
/**
 * @brief the CSerialPortInfo class 串口信息辅助类
 *
 */
class DLL_EXPORT CSerialPortInfo
{
public:
    /**
     * @brief Construct a new CSerialPortInfo object 构造函数
     *
     */
    CSerialPortInfo();
    /**
     * @brief Destroy the CSerialPortInfo object 析构函数
     *
     */
    ~CSerialPortInfo();

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
    static vector<SerialPortInfo> availablePortInfos();
};
} // namespace itas109
#endif //__CSERIALPORTINFO_H__