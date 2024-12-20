#include <windows.h>
#include <initguid.h> // GUID
#include <setupapi.h> // SetupDiGetClassDevs Setup*
#include <tchar.h>    // _T

#include "CSerialPort/iutils.hpp"
#include "CSerialPort/SerialPort_global.h"
#include "CSerialPort/SerialPortInfoWinBase.h"

/********************* EnumDetailsSerialPorts ****************************************/
#undef PHYSICAL_ADDRESS
#define PHYSICAL_ADDRESS LARGE_INTEGER

#if defined(_MSC_VER)
#pragma comment(lib, "setupapi.lib") // SetupDiGetClassDevs Setup*
#pragma comment(lib, "advapi32.lib") // RegQueryValueEx RegCloseKey
#endif

// #include <ntddser.h>  //GUID_DEVINTERFACE_COMPORT
#ifndef GUID_DEVINTERFACE_COMPORT
DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0x86E0D1E0L, 0x8089, 0x11D0, 0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73);
#endif

/**
 * @brief enumDetailsSerialPorts 通过setapi.lib枚举串口详细信息
 * @param portInfoList [out] port info list 串口信息列表
 * @return
 * @retval true true if excute success 执行成功返回true
 * @retval false false if excute failed 执行失败返回false
 */
bool enumDetailsSerialPorts(std::vector<itas109::SerialPortInfo> &portInfoList)
{
    // https://docs.microsoft.com/en-us/windows/win32/api/setupapi/nf-setupapi-setupdienumdeviceinfo

    bool bRet = false;

    HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;

    // Return only devices that are currently present in a system
    // The GUID_DEVINTERFACE_COMPORT device interface class is defined for COM ports. GUID
    // {86E0D1E0-8089-11D0-9CE4-08003E301F73}
    hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_COMPORT, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (INVALID_HANDLE_VALUE != hDevInfo)
    {
        SP_DEVINFO_DATA devInfoData;
        // The caller must set DeviceInfoData.cbSize to sizeof(SP_DEVINFO_DATA)
        devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

        for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++)
        {
            // get port name
            TCHAR portName[256] = {0};
            HKEY hDevKey = SetupDiOpenDevRegKey(hDevInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
            if (INVALID_HANDLE_VALUE != hDevKey)
            {
                DWORD dwCount = 255; // DEV_NAME_MAX_LEN
                RegQueryValueEx(hDevKey, _T("PortName"), NULL, NULL, (BYTE *)portName, &dwCount);
                RegCloseKey(hDevKey);
            }

            // get hardware id
            TCHAR hardwareId[256] = {0};
            SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData, SPDRP_HARDWAREID, NULL, (PBYTE)hardwareId, sizeof(hardwareId), NULL);

            // get friendly name
            TCHAR friendlyName[256] = {0};
            SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)friendlyName, sizeof(friendlyName), NULL);

            itas109::SerialPortInfo m_serialPortInfo;
#ifdef UNICODE
#ifdef CSERIALPORT_USE_UTF8
            itas109::IUtils::WCharToUTF8(m_serialPortInfo.portName, 256, portName);
            itas109::IUtils::WCharToUTF8(m_serialPortInfo.description, 256, friendlyName);
            itas109::IUtils::WCharToUTF8(m_serialPortInfo.hardwareId, 256, hardwareId);
#else
            itas109::IUtils::WCharToNativeMB(m_serialPortInfo.portName, 256, portName);
            itas109::IUtils::WCharToNativeMB(m_serialPortInfo.description, 256, friendlyName);
            itas109::IUtils::WCharToNativeMB(m_serialPortInfo.hardwareId, 256, hardwareId);
#endif
#else
#ifdef CSERIALPORT_USE_UTF8
            wchar_t portNameWChar[256], friendlyNameWChar[256], hardwareIdWChar[256];
            // ANSI to WChar
            itas109::IUtils::NativeMBToWChar(portNameWChar, 256, portName);
            itas109::IUtils::NativeMBToWChar(friendlyNameWChar, 256, friendlyName);
            itas109::IUtils::NativeMBToWChar(hardwareIdWChar, 256, hardwareId);
            // WChar to UTF8
            itas109::IUtils::WCharToUTF8(m_serialPortInfo.portName, 256, portNameWChar);
            itas109::IUtils::WCharToUTF8(m_serialPortInfo.description, 256, friendlyNameWChar);
            itas109::IUtils::WCharToUTF8(m_serialPortInfo.hardwareId, 256, hardwareIdWChar);
#else
            itas109::IUtils::strncpy(m_serialPortInfo.portName, portName, 256);
            itas109::IUtils::strncpy(m_serialPortInfo.description, friendlyName, 256);
            itas109::IUtils::strncpy(m_serialPortInfo.hardwareId, hardwareId, 256);
#endif
#endif

            // get usb device's vid and pid
            if (0 == itas109::IUtils::strFind(m_serialPortInfo.hardwareId, "USB\\"))
            {
                // USB\\VID_1A86&PID_7523&REV_0264
                int vid = -1;
                int pid = -1;
                if (2 == itas109::IUtils::strScan(m_serialPortInfo.hardwareId, "USB\\VID_%04x&PID_%04x", &vid, &pid))
                {
                    itas109::IUtils::strFormat(m_serialPortInfo.hardwareId, 10, "%04x:%04x", vid, pid);
                }
            }

            // get description
            char hardwareIdDesp[256] = {0};
            HardwareIdDespSingleton::getInstance()->getHardwareIdDescription(m_serialPortInfo.hardwareId, hardwareIdDesp);
            if ('\0' == hardwareIdDesp[0])
            {
                // remove (COMxx)
                int index = itas109::IUtils::strFind(m_serialPortInfo.description, " (COM");
                if (-1 != index)
                {
                    // ELTIMA Virtual Serial Port (COM3->COM2)
                    m_serialPortInfo.description[index] = '\0';
                }
            }
            else
            {
                itas109::IUtils::strncpy(m_serialPortInfo.description, hardwareIdDesp, 256);
            }

            portInfoList.push_back(m_serialPortInfo);
        }

        if (ERROR_NO_MORE_ITEMS == GetLastError())
        {
            bRet = true; // no more item
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);

    return bRet;
}
/********************* EnumDetailsSerialPorts ****************************************/

CSerialPortInfoWinBase::CSerialPortInfoWinBase() {}

CSerialPortInfoWinBase::~CSerialPortInfoWinBase() {}

std::vector<itas109::SerialPortInfo> CSerialPortInfoWinBase::availablePortInfos()
{
    std::vector<itas109::SerialPortInfo> portInfoList;
    enumDetailsSerialPorts(portInfoList);

    return portInfoList;
}