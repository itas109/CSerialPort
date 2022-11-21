/**
 * @file cserialport.i
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * SWIG: 4.1.0
 * @brief Python CSerialPort Interface Python的CSerialPort接口
 */
%module(directors="1") cserialport

#define DLL_EXPORT

%{
#include "CSerialPort/SerialPort_global.h"
#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortListener.h"
#include "CSerialPort/SerialPortInfo.h"
%}

%include "std_vector.i" // std::vector
%include "cdata.i"      // cdata memmove for void*
%include "cmalloc.i"    // malloc free for void*
%malloc(void)           // malloc void*
%free(void)             // free void*

%template(SerialPortInfoVector) std::vector<itas109::SerialPortInfo>;

// enable inherit CSerialPortListener interface to receive data
%feature("director") itas109::CSerialPortListener;

%include "CSerialPort/SerialPort_global.h"
%include "CSerialPort/SerialPort.h"
%include "CSerialPort/SerialPortListener.h"
%include "CSerialPort/SerialPortInfo.h"