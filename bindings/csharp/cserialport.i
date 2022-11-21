/**
 * @file cserialport.i
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * SWIG: 4.1.0
 * @brief CSharp CSerialPort Interface  C#的CSerialPort接口
 */
/* File : cserialport.i */
%module(directors="1") cserialportCSharp

#define DLL_EXPORT

%{
#include "CSerialPort/SerialPort_global.h"
#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortListener.h"
#include "CSerialPort/SerialPortInfo.h"
%}

%include "std_vector.i" // std::vector

%include "arrays_csharp.i" // typemaps for csharp
%apply unsigned char INPUT[] {void *}  // void* => byte[]

%template(SerialPortInfoVector) std::vector<itas109::SerialPortInfo>;

// enable inherit CSerialPortListener interface to receive data
%feature("director") itas109::CSerialPortListener;

%include "CSerialPort/SerialPort_global.h"
%include "CSerialPort/SerialPort.h"
%include "CSerialPort/SerialPortListener.h"
%include "CSerialPort/SerialPortInfo.h"