/**
 * @file SerialPortHotPlug_mac.hpp
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief CSerialPort extend class,used to monitor port add and remove CSerialPort的扩展类,用于监测串口的插入与移除
 */
#ifndef __CSERIALPORT_HOTPLUG_MAC_HPP__
#define __CSERIALPORT_HOTPLUG_MAC_HPP__

#include "ithread.hpp"

#include "SerialPortListener.h"

// mac
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>

namespace itas109
{
class CSerialPortHotPlug
{
public:
    CSerialPortHotPlug()
        : m_notificationPort(NULL)
        , m_serialPortAddIterator(0)
        , m_serialPortRemoveIterator(0)
        , m_thread(I_THREAD_INITIALIZER)
        , p_listener(NULL)

    {
        init();
    }
    ~CSerialPortHotPlug()
    {
        if (m_thread != I_THREAD_INITIALIZER)
        {
            i_thread_join(m_thread);
            m_thread = I_THREAD_INITIALIZER;
        }

        if (m_notificationPort != NULL)
        {
            IONotificationPortDestroy(m_notificationPort);
        }
        if (m_serialPortAddIterator)
        {
            IOObjectRelease(m_serialPortAddIterator);
        }
        if (m_serialPortRemoveIterator)
        {
            IOObjectRelease(m_serialPortRemoveIterator);
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
        CSerialPortHotPlug *p_base = (CSerialPortHotPlug *)param;

        if (p_base)
        {
            // create notificaiton
            p_base->m_notificationPort = IONotificationPortCreate(kIOMasterPortDefault);
            if (!p_base->m_notificationPort)
            {
                // fprintf(stderr, "Failed to create IONotificationPort.\n");
                return NULL;
            }
            // create run loop
            CFRunLoopSourceRef runLoopSource = IONotificationPortGetRunLoopSource(p_base->m_notificationPort);
            CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopDefaultMode);

            kern_return_t result;

            // match add serial
            CFMutableDictionaryRef matchingAddDict = IOServiceMatching(kIOSerialBSDServiceValue);
            if (!matchingAddDict)
            {
                // fprintf(stderr, "Failed to create matching add dictionary.\n");
                IONotificationPortDestroy(p_base->m_notificationPort);
                return NULL;
            }
            // register serial add event
            result = IOServiceAddMatchingNotification(p_base->m_notificationPort, kIOPublishNotification, matchingAddDict, serialPortAdd, p_base, &p_base->m_serialPortAddIterator);
            if (result != KERN_SUCCESS)
            {
                // fprintf(stderr, "Failed to add publish notification: %d.\n", result);
                CFRelease(matchingAddDict);
                IONotificationPortDestroy(p_base->m_notificationPort);
                return NULL;
            }
            // deal current device
            serialPortAdd(p_base, p_base->m_serialPortAddIterator);
            // clear dirct
            // CFRelease(matchingAddDict); // TODO

            // match remove serial
            CFMutableDictionaryRef matchingRemoveDict = IOServiceMatching(kIOSerialBSDServiceValue);
            if (!matchingRemoveDict)
            {
                // fprintf(stderr, "Failed to create matching add dictionary.\n");
                IONotificationPortDestroy(p_base->m_notificationPort);
                return NULL;
            }
            // register serial remove event
            result = IOServiceAddMatchingNotification(p_base->m_notificationPort, kIOTerminatedNotification, matchingRemoveDict, serialPortRemove, p_base, &p_base->m_serialPortRemoveIterator);
            if (result != KERN_SUCCESS)
            {
                // fprintf(stderr, "Failed to add terminated notification: %d.\n", result);
                CFRelease(matchingRemoveDict);
                IONotificationPortDestroy(p_base->m_notificationPort);
                return NULL;
            }
            // deal current device
            serialPortRemove(p_base, p_base->m_serialPortRemoveIterator);
            // clear dict
            // CFRelease(matchingRemoveDict); // TODO

            // message loop (could not delete)
            CFRunLoopRun();
        }
    }

private:
    bool init()
    {
        itas109::i_thread_create(&m_thread, NULL, threadFun, (void *)this);
        return true;
    }

private:
    static void serialPortAdd(void *refCon, io_iterator_t iterator)
    {
        CSerialPortHotPlug *p_base = (CSerialPortHotPlug *)refCon;
        if (p_base)
        {
            io_object_t serialPort;
            while ((serialPort = IOIteratorNext(iterator)))
            {
                char portName[256];
                CFStringRef cfString = (CFStringRef)IORegistryEntryCreateCFProperty(serialPort, CFSTR(kIOCalloutDeviceKey), kCFAllocatorDefault, 0);
                if (cfString != NULL)
                {
                    CFStringGetCString(cfString, portName, sizeof(portName), kCFStringEncodingUTF8);
                    CFRelease(cfString);
                    if (p_base->p_listener)
                    {
                        p_base->p_listener->onHotPlugEvent(portName, 1);
                    }
                }
                IOObjectRelease(serialPort);
            }
        }
    }

    static void serialPortRemove(void *refCon, io_iterator_t iterator)
    {
        CSerialPortHotPlug *p_base = (CSerialPortHotPlug *)refCon;
        if (p_base)
        {
            io_object_t serialPort;
            while ((serialPort = IOIteratorNext(iterator)))
            {
                char portName[256];
                CFStringRef cfString = (CFStringRef)IORegistryEntryCreateCFProperty(serialPort, CFSTR(kIOCalloutDeviceKey), kCFAllocatorDefault, 0);
                if (cfString != NULL)
                {
                    CFStringGetCString(cfString, portName, sizeof(portName), kCFStringEncodingUTF8);
                    CFRelease(cfString);
                    if (p_base->p_listener)
                    {
                        p_base->p_listener->onHotPlugEvent(portName, 0);
                    }
                }
                IOObjectRelease(serialPort);
            }
        }
    }

private:
    IONotificationPortRef m_notificationPort;
    io_iterator_t m_serialPortAddIterator;
    io_iterator_t m_serialPortRemoveIterator;

    itas109::i_thread_t m_thread;
    itas109::CSerialPortHotPlugListener *p_listener;
};
} // namespace itas109

#endif