/**
 * @file SerialPortHotPlug_linux.hpp
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief CSerialPort extend class,used to monitor port add and remove CSerialPort的扩展类,用于监测串口的插入与移除
 */
#ifndef __CSERIALPORT_HOTPLUG_LINUX_HPP__
#define __CSERIALPORT_HOTPLUG_LINUX_HPP__

#include "ithread.hpp"

#include "SerialPortListener.h"

// linux
// #include <iostream> // std::cout
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>

namespace itas109
{
class CSerialPortHotPlug
{
public:
    CSerialPortHotPlug()
        : m_thread(NULL)
        , p_listener(NULL)
        , m_sock(-1)
    {
        init();
    }
    ~CSerialPortHotPlug()
    {
        if (m_thread != NULL)
        {
            i_thread_join(m_thread);
            m_thread = NULL;
        }

        if (m_sock != -1)
        {
            close(m_sock);
        }
    }

    int connectHotPlugEvent(itas109::CSerialPortHotPlugListener *event)
    {
        if (event)
        {
            p_listener = event;
            return 0;
        }
        else
        {
            return -1;
        }
        return 0;
    }

    int disconnectHotPlugEvent()
    {
        p_listener = NULL;
        return 0;
    }

private:
    static void *threadFun(void *param)
    {
        CSerialPortHotPlug *p_main = (CSerialPortHotPlug *)param;

        char buffer[2048]; // UEVENT_BUFFER_SIZE
        for (;;)
        {
            ssize_t len = recv(p_main->m_sock, buffer, sizeof(buffer), 0);
            if (len < 0)
            {
                continue;
            }

            // parse
            std::string message(buffer, len);

            // printf can not output all string
            // std::cout << message << std::endl;

            // add@/devices/pci0000:00/0000:00:11.0/0000:02:00.0/usb2/2-2/2-2.1/2-2.1:1.0/ttyUSB0/tty/ttyUSB0ACTION=addDEVPATH=/devices/pci0000:00/0000:00:11.0/0000:02:00.0/usb2/2-2/2-2.1/2-2.1:1.0/ttyUSB0/tty/ttyUSB0SUBSYSTEM=ttyMAJOR=188MINOR=0DEVNAME=ttyUSB0SEQNUM=8128
            // libudev((ACTION=addDEVPATH=/devices/pci0000:00/0000:00:11.0/0000:02:00.0/usb2/2-2/2-2.1/2-2.1:1.0/ttyUSB0/tty/ttyUSB0SUBSYSTEM=ttyDEVNAME=/dev/ttyUSB0SEQNUM=8128USEC_INITIALIZED=5092130519MAJOR=188MINOR=0ID_BUS=usbID_VENDOR_ID=1a86ID_MODEL_ID=7523ID_PCI_CLASS_FROM_DATABASE=Serial
            // bus controllerID_PCI_SUBCLASS_FROM_DATABASE=USB controllerID_PCI_INTERFACE_FROM_DATABASE=UHCIID_VENDOR_FROM_DATABASE=QinHeng ElectronicsID_MODEL_FROM_DATABASE=CH340
            // serial
            // converterID_VENDOR=1a86ID_VENDOR_ENC=1a86ID_MODEL=USB_SerialID_MODEL_ENC=USB\x20SerialID_REVISION=0264ID_SERIAL=1a86_USB_SerialID_TYPE=genericID_USB_INTERFACES=:ff0102:ID_USB_INTERFACE_NUM=00ID_USB_DRIVER=ch341ID_USB_CLASS_FROM_DATABASE=Vendor
            // Specific Class.ID_PORT=0ID_PATH=pci-0000:02:00.0-usb-0:2.1:1.0ID_PATH_TAG=pci-0000_02_00_0-usb-0_2_1_1_0DEVLINKS=/dev/serial/by-path/pci-0000:02:00.0-usb-0:2.1:1.0-port0
            // /dev/serial/by-id/usb-1a86_USB_Serial-if00-port0ID_MM_CANDIDATE=1TAGS=:systemd:CURRENT_TAGS=:systemd:
            size_t subsystem_pos = message.find("SUBSYSTEM=tty");
            if (subsystem_pos == std::string::npos)
            {
                continue;
            }

            // printf can not output all string
            // std::cout << message << std::endl;

            // Find necessary substrings
            size_t add_action_pos = message.find("ACTION=add");
            size_t remove_action_pos = message.find("ACTION=remove");
            size_t devname_pos = message.find("DEVNAME=/"); // / is match /dev/ttyUSB0

            if (add_action_pos != std::string::npos && devname_pos != std::string::npos)
            {
                std::string devname = message.substr(devname_pos + strlen("DEVNAME="));
                // printf("add %s\n", devname.c_str());

                if (p_main && p_main->p_listener)
                {
                    p_main->p_listener->onHotPlugEvent(devname.c_str(), 1);
                }
            }
            else if (remove_action_pos != std::string::npos && devname_pos != std::string::npos)
            {
                std::string devname = message.substr(devname_pos + strlen("DEVNAME="));
                // printf("remove %s\n", devname.c_str());

                if (p_main && p_main->p_listener)
                {
                    p_main->p_listener->onHotPlugEvent(devname.c_str(), 0);
                }
            }
        }
    }

private:
    bool init()
    {
        struct sockaddr_nl addr;

        m_sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
        if (m_sock < 0)
        {
            return false;
        }

        std::memset(&addr, 0, sizeof(addr));
        addr.nl_family = AF_NETLINK;
        addr.nl_groups = -1;

        if (bind(m_sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0)
        {
            close(m_sock);
            return false;
        }

        itas109::i_thread_create(&m_thread, NULL, threadFun, (void *)this);

        return true;
    }

private:
    int m_sock;
    itas109::i_thread_t m_thread;
    itas109::CSerialPortHotPlugListener *p_listener;
};
} // namespace itas109

#endif