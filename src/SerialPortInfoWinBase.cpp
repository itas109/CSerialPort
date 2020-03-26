#include "SerialPortInfoWinBase.h"

#include "Windows.h"

/********************* EnumDetailsSerialPorts ****************************************/
#undef PHYSICAL_ADDRESS
#define PHYSICAL_ADDRESS LARGE_INTEGER

#include <initguid.h>//GUID GUID_DEVINTERFACE_COMPORT
#include <Setupapi.h>//SetupDiGetClassDevs Setup*
#include <tchar.h>//_T
/********************* EnumDetailsSerialPorts ****************************************/

CSerialPortInfoWinBase* CSerialPortInfoWinBase::p_serialPortInfoWinBase;

CSerialPortInfoWinBase::CSerialPortInfoWinBase()
{
    p_serialPortInfoWinBase = this;
}


CSerialPortInfoWinBase::~CSerialPortInfoWinBase()
{
}



std::list<std::string> CSerialPortInfoWinBase::availablePorts()
{
    std::list<std::string> portsList;
    ///XP/Win7/Win10系统的注册表位置，其他系统根据实际情况做修改
    std::string m_regKeyPath = std::string("HARDWARE\\DEVICEMAP\\SERIALCOMM");
    p_serialPortInfoWinBase->getRegKeyValues(m_regKeyPath, portsList);
    return portsList;
}

std::list<std::string> CSerialPortInfoWinBase::availableFriendlyPorts()
{
    std::list<std::string> portsList;

    // Win2k and later support a standard API for
    // enumerating hardware devices.
    p_serialPortInfoWinBase->enumDetailsSerialPorts(portsList);

    return portsList;
}

bool CSerialPortInfoWinBase::getRegKeyValues(std::string regKeyPath, std::list<std::string> & portsList)
{
    //https://msdn.microsoft.com/en-us/library/ms724256

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

    HKEY hKey;

    TCHAR		achValue[MAX_VALUE_NAME];					// buffer for subkey name
    DWORD		cchValue = MAX_VALUE_NAME;					// size of name string
    TCHAR		achClass[MAX_PATH] = TEXT("");				// buffer for class name
    DWORD		cchClassName = MAX_PATH;					// size of class string
    DWORD		cSubKeys = 0;								// number of subkeys
    DWORD		cbMaxSubKey;								// longest subkey size
    DWORD		cchMaxClass;								// longest class string
    DWORD		cKeyNum;									// number of values for key
    DWORD		cchMaxValue;								// longest value name
    DWORD		cbMaxValueData;								// longest value data
    DWORD		cbSecurityDescriptor;						// size of security descriptor
    FILETIME	ftLastWriteTime;							// last write time

    int iRet = -1;
    bool bRet = false;

    std::string m_keyValue;

    TCHAR m_regKeyPath[MAX_KEY_LENGTH];

    TCHAR strDSName[MAX_VALUE_NAME];
    memset(strDSName, 0, MAX_VALUE_NAME);
    DWORD nValueType = 0;
    DWORD nBuffLen = 10;

#ifdef UNICODE
    int iLength;
    const char * _char = regKeyPath.c_str();
    iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
    MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, m_regKeyPath, iLength);
#else
    strcpy_s(m_regKeyPath, MAX_KEY_LENGTH, regKeyPath.c_str());
#endif

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, m_regKeyPath, 0, KEY_READ, &hKey))
    {
        // Get the class name and the value count.
        iRet = RegQueryInfoKey(
                    hKey,                    // key handle
                    achClass,                // buffer for class name
                    &cchClassName,           // size of class string
                    NULL,                    // reserved
                    &cSubKeys,               // number of subkeys
                    &cbMaxSubKey,            // longest subkey size
                    &cchMaxClass,            // longest class string
                    &cKeyNum,                // number of values for this key
                    &cchMaxValue,            // longest value name
                    &cbMaxValueData,         // longest value data
                    &cbSecurityDescriptor,   // security descriptor
                    &ftLastWriteTime);       // last write time

        if (!portsList.empty())
        {
            portsList.clear();
        }

        // Enumerate the key values.
        if (cKeyNum > 0 && ERROR_SUCCESS == iRet)
        {
            for (int i = 0; i < (int)cKeyNum; i++)
            {
                cchValue = MAX_VALUE_NAME;
                achValue[0] = '\0';
                nBuffLen = MAX_KEY_LENGTH;//防止 ERROR_MORE_DATA 234L 错误

                if (ERROR_SUCCESS == RegEnumValue(hKey, i, achValue, &cchValue, NULL, NULL, (LPBYTE)strDSName, &nBuffLen))
                {

#ifdef UNICODE
                    int iLen = WideCharToMultiByte(CP_ACP, 0, strDSName, -1, NULL, 0, NULL, NULL);
                    char* chRtn = new char[iLen * sizeof(char)];
                    WideCharToMultiByte(CP_ACP, 0, strDSName, -1, chRtn, iLen, NULL, NULL);
                    m_keyValue = std::string(chRtn);
                    delete chRtn;
                    chRtn = NULL;
#else
                    m_keyValue = std::string(strDSName);
#endif
                    portsList.push_back(m_keyValue);
                }
            }
        }
        else
        {

        }
    }

    if (portsList.empty())
    {
        bRet = false;
    }
    else
    {
        bRet = true;
    }


    RegCloseKey(hKey);

    return bRet;
}

bool CSerialPortInfoWinBase::enumDetailsSerialPorts(std::list<std::string> &portsList)
{
    // https://docs.microsoft.com/en-us/windows/win32/api/setupapi/

    bool bRet = false;

    std::string strFriendlyName;
    std::string strPortName;

    HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
    SP_DEVICE_INTERFACE_DETAIL_DATA *pDetData = NULL;

    // Return only devices that are currently present in a system
    // The GUID_DEVINTERFACE_COMPORT device interface class is defined for COM ports. GUID {86E0D1E0-8089-11D0-9CE4-08003E301F73}
    hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_COMPORT, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (INVALID_HANDLE_VALUE != hDevInfo)
    {
        BOOL bOk = TRUE;

        DWORD dwDetDataSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA)+256;
        pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)new char[dwDetDataSize];

        if (pDetData != NULL)
        {
            pDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

            SP_DEVICE_INTERFACE_DATA did = { 0 };
            did.cbSize = sizeof(did);

            for (DWORD idev = 0; bOk; idev++)
            {
                // enumerates the device interfaces that are contained in a device information set
                bOk = SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVINTERFACE_COMPORT, idev, &did);
                if (bOk)
                {
                    //DWORD cbRequired = 0;

                    SP_DEVINFO_DATA devdata = { sizeof(SP_DEVINFO_DATA) };
                    // get detailed information about an interface
                    //bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo, &did, NULL, NULL,&cbRequired, &devdata);

                    bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo, &did, pDetData, dwDetDataSize, NULL, &devdata);

                    if (bOk)
                    {
                        TCHAR fname[256];
                        // retrieves a specified Plug and Play device property (include friendlyname etc.)
                        BOOL bSuccess = SetupDiGetDeviceRegistryProperty(hDevInfo, &devdata, SPDRP_FRIENDLYNAME, NULL, (PBYTE)fname, sizeof(fname), NULL);

                        TCHAR portName[256];
                        // get port name
                        HKEY hDevKey = SetupDiOpenDevRegKey(hDevInfo, &devdata, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
                        if (INVALID_HANDLE_VALUE != hDevKey)
                        {
                            DWORD dwCount = 255;//DEV_NAME_MAX_LEN
                            RegQueryValueEx(hDevKey, _T("PortName"), NULL, NULL, (BYTE*)portName, &dwCount);
                            RegCloseKey(hDevKey);

                            bSuccess &= TRUE;
                        }
                        else
                        {
                            bSuccess &= FALSE;
                        }

                        if (bSuccess)
                        {

#ifdef UNICODE
                            int iLen = WideCharToMultiByte(CP_ACP, 0, portName, -1, NULL, 0, NULL, NULL);
                            char* chRtn = new char[iLen * sizeof(char)];
                            WideCharToMultiByte(CP_ACP, 0, portName, -1, chRtn, iLen, NULL, NULL);
                            strPortName = std::string(chRtn);
                            delete chRtn;
                            chRtn = NULL;
#else
                            strPortName = std::string(portName);
#endif

#ifdef UNICODE
                            int iLen2 = WideCharToMultiByte(CP_ACP, 0, fname, -1, NULL, 0, NULL, NULL);
                            char* chRtn2 = new char[iLen2 * sizeof(char)];
                            WideCharToMultiByte(CP_ACP, 0, fname, -1, chRtn2, iLen2, NULL, NULL);
                            strFriendlyName = std::string(chRtn2);
                            delete chRtn2;
                            chRtn2 = NULL;
#else
                            strFriendlyName = std::string(fname);
#endif
                            // remove (COMxx)
                            strFriendlyName = strFriendlyName.substr(0, strFriendlyName.find(("(COM")));
                            // compose string => COM1 Prolific USB-to-Serial Comm Port
                            portsList.push_back(strPortName + " " + strFriendlyName);

                            bRet = true;
                        }

                    }
                    else
                    {
                        bRet = false;
                    }
                }
                else if (ERROR_NO_MORE_ITEMS == GetLastError())
                {
                    bRet = false;// Enumeration failed
                    break;
                }
                else
                {

                }
            }

            delete[](char*)pDetData;
            pDetData = NULL;
        }
        else
        {
            bRet = false;
        }
        SetupDiDestroyDeviceInfoList(hDevInfo);
    }

    return bRet;
}

