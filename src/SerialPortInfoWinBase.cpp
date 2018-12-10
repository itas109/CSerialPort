#include "SerialPortInfoWinBase.h"

#include "Windows.h"

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
	///仅是XP/Win7系统的注册表位置，其他系统根据实际情况做修改
	std::string m_regKeyPath = std::string("HARDWARE\\DEVICEMAP\\SERIALCOMM");
	p_serialPortInfoWinBase->getRegKeyValues(m_regKeyPath, portsList);
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
