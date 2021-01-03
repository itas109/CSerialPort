/**
 * @file SerialPortInfoBase.h
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * QQ Group : 12951803
 * @brief the CSerialPortInfo Base class 串口信息辅助类基类
 * @date 2020-04-29
 * @copyright The CSerialPort is Copyright (C) 2021 itas109. \n
 * Contact: itas109@qq.com \n\n
 *  You may use, distribute and copy the CSerialPort under the terms of \n
 *  GNU Lesser General Public License version 3, which is displayed below.
 */
#ifndef __CSERIALPORTINFOBASE_H__
#define __CSERIALPORTINFOBASE_H__

#include <string>
#include <vector>

#include <list>

using namespace std;

struct SerialPortInfo
{
    std::string portName;
    std::string description;
};

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
     * @brief availablePortInfos 获取串口信息列表
     * @return return available port infolist 返回可用串口名称列表
     */
    static std::vector<SerialPortInfo> availablePortInfos(void);
};
#endif //__CSERIALPORTINFOBASE_H__
