/**
 * @file SerialPortInfoBase.h
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief the CSerialPortInfo Base class 串口信息辅助类基类
 * @copyright The CSerialPort is Copyright (C) 2014 itas109 <itas109@qq.com>. \n
 * You may use, copy, modify, and distribute the CSerialPort, under the terms \n
 * of the LICENSE file.
 */
#ifndef __CSERIALPORTINFOBASE_H__
#define __CSERIALPORTINFOBASE_H__

#include <vector>
#include <map>

namespace itas109
{
struct SerialPortInfo; // Forward Declaration
}

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
    static std::vector<itas109::SerialPortInfo> availablePortInfos(void);
};

class lessConstString
{
public:
    bool operator()(const char *lhs, const char *rhs) const;
};

typedef std::map<const char *, const char *, lessConstString> HardwareIdDespMap;

class HardwareIdDespSingleton
{
public:
    static HardwareIdDespSingleton *getInstance();
    void getHardwareIdDescription(const char *hardwareId, char *hardwareDesp);

private:
    HardwareIdDespSingleton();
    ~HardwareIdDespSingleton();
    HardwareIdDespSingleton(const HardwareIdDespSingleton &instance);
    HardwareIdDespSingleton &operator=(const HardwareIdDespSingleton &instance);

private:
    static HardwareIdDespSingleton g_instance;
    HardwareIdDespMap m_hwIdMap;
};
#endif //__CSERIALPORTINFOBASE_H__
