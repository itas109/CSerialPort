/**
 * @file SerialPortInfoUnixBase.h
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * QQ Group : 12951803
 * @brief the CSerialPortInfo unix class unix串口信息辅助类基类
 * @todo Not implemented 未实现
 * @date 2020-04-29
 * @copyright The CSerialPort is Copyright (C) 2021 itas109. \n
 * Contact: itas109@qq.com \n\n
 *  You may use, distribute and copy the CSerialPort under the terms of \n
 *  GNU Lesser General Public License version 3, which is displayed below.
 */
#ifndef __CSERIALPORTINFOUNIXBASE_H__
#define __CSERIALPORTINFOUNIXBASE_H__

#include "SerialPortInfoBase.h"

/**
 * @brief the CSerialPortInfo unix class unix串口信息辅助类基类
 *
 */
class CSerialPortInfoUnixBase : public CSerialPortInfoBase
{
public:
    CSerialPortInfoUnixBase();
    ~CSerialPortInfoUnixBase();

    /**
     * @brief availablePortInfos 获取串口信息列表
     * @return return available port infolist 返回可用串口名称列表
     */
    static std::vector<SerialPortInfo> availablePortInfos();
};
#endif //__CSERIALPORTINFOUNIXBASE_H__
