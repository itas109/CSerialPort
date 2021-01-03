/**
 * @file SerialPortInfo.h
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * QQ Group : 12951803
 * @brief the CSerialPortInfo class 串口信息辅助类
 * @date 2020-04-29
 * @copyright The CSerialPort is Copyright (C) 2021 itas109. \n
 * Contact: itas109@qq.com \n\n
 *  You may use, distribute and copy the CSerialPort under the terms of \n
 *  GNU Lesser General Public License version 3, which is displayed below.
 */
#ifndef __CSERIALPORTINFO_H__
#define __CSERIALPORTINFO_H__

#include "SerialPortInfoBase.h"
#include "SerialPort_global.h"

#include <list>
#include <string>

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
     * @brief availablePortInfos 获取串口信息列表
     * @return return available port infolist 返回可用串口名称列表
     */
    static vector<SerialPortInfo> availablePortInfos();
};
} // namespace itas109
#endif //__CSERIALPORTINFO_H__