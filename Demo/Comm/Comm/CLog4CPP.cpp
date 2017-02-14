#include "stdafx.h"
#include "CLog4CPP.h"

CLog4CPP::CLog4CPP()
{
	m_bEnable = false;
	m_bPrintTime = true;//默认加入时间戳
	m_csFileName = "";

	//初始化临界区
	InitializeCriticalSection(&m_crit);
}

CLog4CPP::~CLog4CPP()
{
	//释放临界区
	DeleteCriticalSection(&m_crit);
}

void CLog4CPP::Init()
{
	m_bEnable = true;

	CTime t = CTime::GetCurrentTime();

	std::string pOutputFilename;
	//用时间命名文件名称
	TCHAR buffer[256] = {0};
	_stprintf_s(buffer, 256,_T("%04d-%02d-%02d.log"), t.GetYear(), t.GetMonth(), t.GetDay());
	pOutputFilename = std::string(buffer);
	
	//获取程序路径和名称
	TCHAR m_ctsFileName[MAX_PATH] = {0};

	DWORD res = GetModuleFileName(GetModuleHandle(0), m_ctsFileName, MAX_PATH);

	std::string path;

#ifdef UNICODE
	int iLen = WideCharToMultiByte(CP_ACP, 0,m_ctsFileName, -1, NULL, 0, NULL, NULL);
	char* chRtn =new char[iLen*sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, m_ctsFileName, -1, chRtn, iLen, NULL, NULL);
	path = std::string(chRtn);
	delete chRtn;
	chRtn = NULL;
#else
	path = std::string(m_ctsFileName);
#endif
	std::string appDir = GetBaseDir(path);

	std::string logDir = appDir + "\\Log";

	_mkdir(logDir.c_str());//创建Log文件夹
	
	m_csFileName = appDir + "\\Log\\" + pOutputFilename;
}

void CLog4CPP::Init(std::string pOutputFilename)
{
	m_bEnable = true;

	if(GetFileExtensions(pOutputFilename) == "")
	{
		pOutputFilename += ".log";
	}

	//获取程序路径和名称
	TCHAR m_ctsFileName[MAX_PATH] = {0};

	//DWORD res = GetModuleFileName(AfxGetInstanceHandle(), m_ctsFileName, MAX_PATH);

	DWORD res = GetModuleFileName(GetModuleHandle(0), m_ctsFileName, MAX_PATH);

	std::string path;

#ifdef UNICODE
	int iLen = WideCharToMultiByte(CP_ACP, 0,m_ctsFileName, -1, NULL, 0, NULL, NULL);
	char* chRtn =new char[iLen*sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, m_ctsFileName, -1, chRtn, iLen, NULL, NULL);
	path = std::string(chRtn);
	delete chRtn;
	chRtn = NULL;
#else
	path = std::string(m_ctsFileName);
#endif
	std::string appDir = GetBaseDir(path);

	std::string logDir = appDir + "\\Log";

	_mkdir(logDir.c_str());//创建Log文件夹

	m_csFileName = appDir + "\\Log\\" + pOutputFilename;
}

void CLog4CPP::IsEnable(bool bEnable)
{
	m_bEnable = bEnable;
}

bool CLog4CPP::LogOut(std::string text)
{
	if (m_csFileName.size() == 0)
		return false;

	if (!m_bEnable)
		return true;

	//if (!AfxIsValidString(text, -1))
	//	return false;

	EnterCriticalSection(&m_crit);

	bool bOK = false;

	// output 
	FILE *fp = NULL;
	TCHAR wc[MAX_PATH] = {0};
#ifdef UNICODE
	_stprintf_s(wc, MAX_PATH, _T("%S"), m_csFileName.c_str());//%S宽字符
#else
	_stprintf_s(wc, MAX_PATH, _T("%s"), m_csFileName.c_str());//%s单字符
#endif
	_tfopen_s(&fp, wc, _T("a"));
	if (fp)
	{
		if (m_bPrintTime)
		{
			CTime t ; 
			t = CTime::GetCurrentTime();
			char buffer[256] = {0};
			sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),t.GetSecond());
			std::string time = std::string(buffer);
#ifdef UNICODE
			_ftprintf_s(fp,_T("%S : "),time.c_str());
#else
			_ftprintf_s(fp,_T("%s : "),time.c_str());
#endif
		}
#ifdef UNICODE
		_ftprintf_s(fp, _T("%S\n"), text.c_str());
#else
		_ftprintf_s(fp, _T("%s\n"), text.c_str());
#endif

		fclose(fp);

		fp = NULL;

		bOK = true;
	}

	LeaveCriticalSection(&m_crit);

	return bOK;
}

std::string	CLog4CPP::GetFileExtensions(std::string &fileName)
{
	std::string out = fileName;

	int iSlashPos = fileName.find_last_of('\\');
	if (iSlashPos !=-1)
	{
		out = out.substr(iSlashPos+1);
	}
	else
	{
		iSlashPos = fileName.find_last_of('/');
		if (iSlashPos !=-1) 
		{
			out = out.substr(iSlashPos+1);
		}
	}

	int iDotPos = out.find_last_of('.');
	if (iDotPos>0)
	{
		out = out.substr(iDotPos);
	}
	else
	{
		out = "";
	}

	return out;
}

std::string CLog4CPP::GetBaseDir(std::string & path)
{
	std::string out = path;
	int iSlashPos = path.find_last_of('\\');
	if (iSlashPos !=-1) 
	{
		out = path.substr(0,iSlashPos);
	} 
	else
	{
		iSlashPos = path.find_last_of('/');
		if (iSlashPos !=-1) 
		{
			out = path.substr(0,iSlashPos);
		} 
	}

	return out;
}