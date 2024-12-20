/**
 * @file SerialPortHotPlug_win.hpp
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief CSerialPort extend class,used to monitor port add and remove CSerialPort的扩展类,用于监测串口的插入与移除
 */
#ifndef __CSERIALPORT_HOTPLUG_WIN_HPP__
#define __CSERIALPORT_HOTPLUG_WIN_HPP__

#include "ithread.hpp"

#include "SerialPortListener.h"

// win32
#include <dbt.h>
#include <tchar.h> // _T
#define CLASS_NAME _T("itas109SerialPortChangeWnd")

#if defined(_MSC_VER)
#pragma comment(lib, "user32.lib") // CreateWindowEx GetMessage DispatchMessage RegisterClassEx UnregisterClass DefWindowProc SetWindowLong GetWindowLong
#endif

namespace itas109
{
class CSerialPortHotPlug
{
public:
    CSerialPortHotPlug()
        : m_thread(NULL)
        , p_listener(NULL)
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
        UnregisterClass(CLASS_NAME, GetModuleHandle(NULL));
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
    static unsigned int __stdcall threadFun(void *param)
    {
        CSerialPortHotPlug *p_main = (CSerialPortHotPlug *)param;

        // create hidden window to receive device change messages
        if (NULL == CreateWindowEx(0, CLASS_NAME, 0, 0, 0, 0, 0, 0, 0 /*HWND_MESSAGE*/, 0, GetModuleHandle(NULL), p_main))
        {
            return -1;
        }

        // message loop
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0))
        {
            DispatchMessage(&msg);
        }

        return 0;
    }

private:
    bool init()
    {
        // https://docs.microsoft.com/zh-cn/windows/win32/devio/registering-for-device-notification
        WNDCLASSEX wndClass = {0};

        HINSTANCE hInstance = GetModuleHandle(NULL);

        if (NULL == hInstance)
        {
            return false;
        }

        wndClass.cbSize = sizeof(WNDCLASSEX);
        wndClass.lpszClassName = CLASS_NAME;
        wndClass.lpfnWndProc = reinterpret_cast<WNDPROC>(winProcCallback);
        wndClass.hInstance = hInstance;

        if (!RegisterClassEx(&wndClass))
        {
            return false;
        }

        itas109::i_thread_create(&m_thread, NULL, threadFun, (LPVOID)this);
        return true;
    }

private:
    static LRESULT CALLBACK winProcCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
            case WM_CREATE:
                SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT *)lParam)->lpCreateParams);
                break;
            case WM_DEVICECHANGE:
            {
                PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;

                if (lpdb && DBT_DEVTYP_PORT == lpdb->dbch_devicetype)
                {
                    CSerialPortHotPlug *p_base = (CSerialPortHotPlug *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
                    if (p_base && p_base->p_listener)
                    {
                        PDEV_BROADCAST_PORT pDevInf = (PDEV_BROADCAST_PORT)lpdb;

                        if (DBT_DEVICEARRIVAL == wParam)
                        {
#ifdef UNICODE
                            char portName[256];
#ifdef CSERIALPORT_USE_UTF8
                            p_base->p_listener->onHotPlugEvent(itas109::IUtils::WCharToUTF8(portName, 256, pDevInf->dbcp_name), 1);
#else
                            p_base->p_listener->onHotPlugEvent(itas109::IUtils::WCharToNativeMB(portName, 256, pDevInf->dbcp_name), 1);
#endif
#else
#ifdef CSERIALPORT_USE_UTF8
                            char portNameUTF8[256];
                            wchar_t portNameWChar[256];
                            // ANSI to WChar
                            itas109::IUtils::NativeMBToWChar(portNameWChar, 256, pDevInf->dbcp_name);
                            // WChar to UTF8
                            p_base->p_listener->onHotPlugEvent(itas109::IUtils::WCharToUTF8(portNameUTF8, 256, portNameWChar), 1);
#else
                            p_base->p_listener->onHotPlugEvent(pDevInf->dbcp_name, 1);
#endif
#endif
                        }
                        else if (DBT_DEVICEREMOVECOMPLETE == wParam)
                        {
#ifdef UNICODE
                            char portName[256];
#ifdef CSERIALPORT_USE_UTF8
                            p_base->p_listener->onHotPlugEvent(itas109::IUtils::WCharToUTF8(portName, 256, pDevInf->dbcp_name), 0);
#else
                            p_base->p_listener->onHotPlugEvent(itas109::IUtils::WCharToNativeMB(portName, 256, pDevInf->dbcp_name), 0);
#endif
#else
#ifdef CSERIALPORT_USE_UTF8
                            char portNameUTF8[256];
                            wchar_t portNameWChar[256];
                            // ANSI to WChar
                            itas109::IUtils::NativeMBToWChar(portNameWChar, 256, pDevInf->dbcp_name);
                            // WChar to UTF8
                            p_base->p_listener->onHotPlugEvent(itas109::IUtils::WCharToUTF8(portNameUTF8, 256, portNameWChar), 0);
#else
                            p_base->p_listener->onHotPlugEvent(pDevInf->dbcp_name, 0);
#endif
#endif
                        }
                        else
                        {
                        }
                    }
                }
            }
            break;
            default:
                break;
        }

        // send all other messages on to the default windows handler
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

private:
    itas109::i_thread_t m_thread;
    itas109::CSerialPortHotPlugListener *p_listener;
};

} // namespace itas109

#endif