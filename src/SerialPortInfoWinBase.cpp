#include "CSerialPort/SerialPortInfoWinBase.h"

#include "Windows.h"

/********************* EnumDetailsSerialPorts ****************************************/
#undef PHYSICAL_ADDRESS
#define PHYSICAL_ADDRESS LARGE_INTEGER

#include <Setupapi.h> //SetupDiGetClassDevs Setup*
#include <initguid.h> //GUID
#include <tchar.h>    //_T

//#include <ntddser.h>  //GUID_DEVINTERFACE_COMPORT
#ifndef GUID_DEVINTERFACE_COMPORT
DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0x86E0D1E0L, 0x8089, 0x11D0, 0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73);
#endif

std::string wstringToString(const std::wstring &wstr)
{
    // https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
    if (wstr.empty())
    {
        return std::string();
    }

    int size = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string ret = std::string(size, 0);
    WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &ret[0], size, NULL, NULL); // CP_UTF8

    return ret;
}

/**
 * @brief enumDetailsSerialPorts 通过setapi.lib枚举串口详细信息
 * @param portInfoList [out] port info list 串口信息列表
 * @return
 * @retval true true if excute success 执行成功返回true
 * @retval false false if excute failed 执行失败返回false
 */
bool enumDetailsSerialPorts(vector<SerialPortInfo> &portInfoList)
{
    // https://docs.microsoft.com/en-us/windows/win32/api/setupapi/nf-setupapi-setupdienumdeviceinfo

    bool bRet = false;
    SerialPortInfo m_serialPortInfo;

    std::string strFriendlyName;
    std::string strPortName;

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
            TCHAR portName[256];
            HKEY hDevKey = SetupDiOpenDevRegKey(hDevInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
            if (INVALID_HANDLE_VALUE != hDevKey)
            {
                DWORD dwCount = 255; // DEV_NAME_MAX_LEN
                RegQueryValueEx(hDevKey, _T("PortName"), NULL, NULL, (BYTE *)portName, &dwCount);
                RegCloseKey(hDevKey);
            }

            // get friendly name
            TCHAR fname[256];
            SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)fname,
                                             sizeof(fname), NULL);

#ifdef UNICODE
            strPortName = wstringToString(portName);
            strFriendlyName = wstringToString(fname);
#else
            strPortName = std::string(portName);
            strFriendlyName = std::string(fname);
#endif
            // remove (COMxx)
            strFriendlyName = strFriendlyName.substr(0, strFriendlyName.find(("(COM")));

            m_serialPortInfo.portName = strPortName;
            m_serialPortInfo.description = strFriendlyName;
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

vector<SerialPortInfo> CSerialPortInfoWinBase::availablePortInfos()
{
    vector<SerialPortInfo> portInfoList;
    enumDetailsSerialPorts(portInfoList);

    return portInfoList;
}