/**
 * @file SerialPortInfoUnixBase.h
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief the CSerialPortInfo unix class unix串口信息辅助类基类
 * @todo Not implemented 未实现
 * @copyright The CSerialPort is Copyright (C) 2014 itas109 <itas109@qq.com>. \n
 * You may use, copy, modify, and distribute the CSerialPort, under the terms \n
 * of the LICENSE file.
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
    static std::vector<itas109::SerialPortInfo> availablePortInfos();
};
#endif //__CSERIALPORTINFOUNIXBASE_H__
