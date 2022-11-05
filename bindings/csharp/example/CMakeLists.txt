#***************************************************************************
# @file CMakeLists.txt
# @author itas109 (itas109@qq.com) \n\n
# Blog : https://blog.csdn.net/itas109 \n
# Github : https://github.com/itas109 \n
# Gitee : https://gitee.com/itas109 \n
# QQ Group : 129518033
# @brief Lightweight cross-platform serial port library based on C++
# @copyright The CSerialPort is Copyright (C) 2014 itas109 <itas109@qq.com>.
# You may use, copy, modify, and distribute the CSerialPort, under the terms
# of the LICENSE file.
############################################################################
cmake_minimum_required(VERSION 3.8.2)

project(CommCSharp LANGUAGES CSharp)

set(ALL_SRCS
    ${CMAKE_SWIG_OUTDIR}/cserialportCSharp.cs
    ${CMAKE_SWIG_OUTDIR}/cserialportCSharpPINVOKE.cs
    ${CMAKE_SWIG_OUTDIR}/CSerialPort.cs
    ${CMAKE_SWIG_OUTDIR}/CSerialPortInfo.cs
    ${CMAKE_SWIG_OUTDIR}/CSerialPortListener.cs
    ${CMAKE_SWIG_OUTDIR}/BaudRate.cs
    ${CMAKE_SWIG_OUTDIR}/DataBits.cs
    ${CMAKE_SWIG_OUTDIR}/FlowControl.cs
    ${CMAKE_SWIG_OUTDIR}/OperateMode.cs
    ${CMAKE_SWIG_OUTDIR}/Parity.cs
    ${CMAKE_SWIG_OUTDIR}/StopBits.cs
    ${CMAKE_SWIG_OUTDIR}/SerialPortError.cs
    ${CMAKE_SWIG_OUTDIR}/SerialPortInfo.cs
    ${CMAKE_SWIG_OUTDIR}/SerialPortInfoVector.cs
)

foreach(src ${ALL_SRCS})
set_source_files_properties(${src} PROPERTIES GENERATED TRUE)
endforeach()

add_executable(${PROJECT_NAME} Program.cs ${ALL_SRCS})