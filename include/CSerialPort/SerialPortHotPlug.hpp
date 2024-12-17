/**
 * @file SerialPortHotPlug.hpp
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief CSerialPort extend class,used to monitor port add and remove CSerialPort的扩展类,用于监测串口的插入与移除
 */
#ifndef __CSERIALPORT_HOTPLUG_HPP__
#define __CSERIALPORT_HOTPLUG_HPP__

#include "SerialPort_global.h"

#if defined(I_OS_WIN)
#include "SerialPortHotPlug_win.hpp"
#elif defined(I_OS_LINUX)
#include "SerialPortHotPlug_linux.hpp"
#elif defined(I_OS_MAC)
#include "SerialPortHotPlug_mac.hpp"
#else
// #error "CSerialPort HotPlug unavailable on this platform"
namespace itas109
{
class CSerialPortHotPlugListener;

class CSerialPortHotPlug
{
public:
    CSerialPortHotPlug() {}
    ~CSerialPortHotPlug() {}

    int connectHotPlugEvent(itas109::CSerialPortHotPlugListener *event)
    {
        return 0;
    }

    int disconnectHotPlugEvent()
    {
        return 0;
    }
};
} // namespace itas109
#endif

#endif