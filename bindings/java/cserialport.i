/**
 * @file cserialport.i
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * SWIG: 4.1.0
 * @brief Java CSerialPort Interface  Java的CSerialPort接口
 */
%module(directors="1") cserialportJava

#define DLL_EXPORT

%{
#include "CSerialPort/SerialPort_global.h"
#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortListener.h"
#include "CSerialPort/SerialPortInfo.h"
%}

%include "std_vector.i" // std::vector

%include "various.i" // typemaps for java
%apply char* BYTE {void*} // void* => byte[]

%template(SerialPortInfoVector) std::vector<itas109::SerialPortInfo>;

// enable inherit CSerialPortListener interface to receive data
%feature("director") itas109::CSerialPortListener;

%include "CSerialPort/SerialPort_global.h"
%include "CSerialPort/SerialPort.h"
%include "CSerialPort/SerialPortListener.h"
%include "CSerialPort/SerialPortInfo.h"