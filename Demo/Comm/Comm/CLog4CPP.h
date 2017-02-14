/*
**************************************************************************************
**
**	FILENAME			CLog4CPP.h
**
**	PURPOSE				这是一个C++的日志工具类,兼容ASNI和UINCODE编码
**
**	CREATION DATE		2016-08-12
**
**	AUTHOR				itas109
**
**  Blog                http://blog.csdn.net/itas109
**
**  Git                 https://github.com/itas109
**
**************************************************************************************
**  author: itas109  date:2016-08-15
**  Blog：blog.csdn.net/itas109
**  改进
**  1） Init方法增加默认初始化名称
**  2） 精简代码
**  3） 优化编码兼容性
**************************************************************************************
**  author: itas109  date:2016-08-17
**  Blog：blog.csdn.net/itas109
**  改进
**  1） Init方法重载，用时间命名文件名
**  2） 日志文件默认放到Log文件夹下
**  3） 增加int2string方法，用于数字与字符的转换
**************************************************************************************
**  author: itas109  date:2016-08-18
**  Blog：blog.csdn.net/itas109
**  改进
**  1） 去除对MFC的依赖
**  2） 采用C++通用数据类型
**************************************************************************************
**  author: itas109  date:2016-08-26
**  Blog：blog.csdn.net/itas109
**  改进
**  1） 采用通用性的#ifndef替换#pragma once
**  2） 替换一些宏定义
**************************************************************************************
**  author: itas109  date:2016-09-12
**  Blog：blog.csdn.net/itas109
**  改进
**  1） 去除int2string函数
**  2） 时间格式统一为xxxx-xx-xx xx:xx:xx，不足位补0，如2016-09-12 16:28:01
**************************************************************************************
**  author: itas109  date:2016-10-13
**  Blog：blog.csdn.net/itas109
**  改进
**  _mkdir创建文件夹时，参数采用绝对路径，防止自启动时无法创建文件夹
*/
#ifndef CLOG4CPP_H
#define CLOG4CPP_H

#include <string>
#include <sstream>
#include <direct.h>
#include "atltime.h"

class CLog4CPP
{
public:

	CLog4CPP();
	~CLog4CPP();

	//初始化
	void Init(std::string stringpOutputFilename);
	void Init();//按照当天日期生成日志文件

	//输出文字，类似与TRACE、printf
	bool LogOut(std::string text);

	//设置使能
	void IsEnable(bool bEnable);

	//是否在每行加入时间戳
	void IsPrintTime(bool b) {m_bPrintTime = b;}

protected:
	//缓冲区
	enum {TBUF_SIZE = 3000};
	std::string m_tBuf[TBUF_SIZE];

	std::string m_csFileName;
	
	//临界区
	CRITICAL_SECTION  m_crit;

	//使能
	bool m_bEnable;
	bool m_bPrintTime;

	//字符工具
	std::string GetBaseDir(std::string & path);
	std::string GetFileExtensions(std::string & fileName);
};
#endif CLOG4CPP_H