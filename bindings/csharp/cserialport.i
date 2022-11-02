/**
 * @file cserialport.i
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief CSharp CSerialPort Interface  C#的CSerialPort接口
 */
/* File : cserialport.i */
%module(directors="1") cserialportCSharp

#define DLL_EXPORT
#define USE_CSERIALPORT_LISTENER

%{
#include "CSerialPort/SerialPort_global.h"
#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortListener.h"
#include "CSerialPort/SerialPortInfo.h"
%}

%include <std_string.i> // std::string
%include <std_vector.i> // std::vector

%apply void *VOID_INT_PTR { void * } // void* => .NET System.IntPtr

%template(SerialPortInfoVector) std::vector<itas109::SerialPortInfo>;

// enable inherit CSerialPortListener interface to receive data
%feature("director") itas109::CSerialPortListener;

// namespace itas109
%nspace itas109::CSerialPort;
%nspace itas109::CSerialPortListener;
%nspace itas109::CSerialPortInfo;
%nspace itas109::SerialPortInfo;
%nspace itas109::OperateMode;
%nspace itas109::BaudRate;
%nspace itas109::DataBits;
%nspace itas109::Parity;
%nspace itas109::StopBits;
%nspace itas109::FlowControl;

%include "CSerialPort/SerialPort_global.h"
%include "CSerialPort/SerialPort.h"
%include "CSerialPort/SerialPortListener.h"
%include "CSerialPort/SerialPortInfo.h"