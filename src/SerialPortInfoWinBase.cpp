#include <Windows.h>
#include <initguid.h> // GUID
#include <Setupapi.h> // SetupDiGetClassDevs Setup*
#include <tchar.h>    // _T

#include "CSerialPort/iutils.hpp"
#include "CSerialPort/SerialPort_global.h"
#include "CSerialPort/SerialPortInfoWinBase.h"

/********************* EnumDetailsSerialPorts ****************************************/
#undef PHYSICAL_ADDRESS
#define PHYSICAL_ADDRESS LARGE_INTEGER

#if defined(_MSC_VER)
#pragma comment(lib, "setupapi.lib")
#endif

//#include <ntddser.h>  //GUID_DEVINTERFACE_COMPORT
#ifndef GUID_DEVINTERFACE_COMPORT
DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0x86E0D1E0L, 0x8089, 0x11D0, 0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73);
#endif

#ifdef UNICODE
static char *WCharToChar(char *dest, const wchar_t *wstr)
{
    if (NULL == wstr)
    {
        return NULL;
    }

    int len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL); // get wchar length
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, dest, len, NULL, NULL);         // CP_UTF8

    return dest;
}
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

            // get friendly name
            TCHAR friendlyName[256] = {0};
            SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)friendlyName, sizeof(friendlyName), NULL);

            // get hardware id
            TCHAR hardwareId[256] = {0};
            SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData, SPDRP_HARDWAREID, NULL, (PBYTE)hardwareId, sizeof(hardwareId), NULL);

            itas109::SerialPortInfo m_serialPortInfo;
#ifdef UNICODE
            char portNameChar[256], friendlyNameChar[256], hardwareIdChar[256];
            itas109::IUtils::strncpy(m_serialPortInfo.portName, WCharToChar(portNameChar, portName), 256);
            itas109::IUtils::strncpy(m_serialPortInfo.description, WCharToChar(friendlyNameChar, friendlyName), 256);
            itas109::IUtils::strncpy(m_serialPortInfo.hardwareId, WCharToChar(hardwareIdChar, hardwareId), 256);
#else
            itas109::IUtils::strncpy(m_serialPortInfo.portName, portName, 256);
            itas109::IUtils::strncpy(m_serialPortInfo.description, friendlyName, 256);
            itas109::IUtils::strncpy(m_serialPortInfo.hardwareId, hardwareId, 256);
#endif
            // remove (COMxx)
            int index = 0;
            while (m_serialPortInfo.description[index])
            {
                if (m_serialPortInfo.description[index] == '(' && m_serialPortInfo.description[index + 1] == 'C' && m_serialPortInfo.description[index + 2] == 'O' &&
                    m_serialPortInfo.description[index + 3] == 'M')
                {
                    m_serialPortInfo.description[index] = '\0';
                    break;
                }

                ++index;
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