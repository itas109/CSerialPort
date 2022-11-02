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
cmake_minimum_required(VERSION 2.8.12)

project(CommCSharp LANGUAGES CSharp)

# file(GLOB_RECURSE ALL_SRCS ../generate/itas109/*.cs ../generate/*.cs)
# source_group(generate ${ALL_SRCS})

set(ALL_SRCS
${PROJECT_SOURCE_DIR}/../generate/itas109/CSerialPort.cs
${PROJECT_SOURCE_DIR}/../generate/itas109/CSerialPortInfo.cs
${PROJECT_SOURCE_DIR}/../generate/itas109/CSerialPortListener.cs
${PROJECT_SOURCE_DIR}/../generate/itas109/BaudRate.cs
${PROJECT_SOURCE_DIR}/../generate/itas109/DataBits.cs
${PROJECT_SOURCE_DIR}/../generate/itas109/FlowControl.cs
${PROJECT_SOURCE_DIR}/../generate/itas109/OperateMode.cs
${PROJECT_SOURCE_DIR}/../generate/itas109/Parity.cs
${PROJECT_SOURCE_DIR}/../generate/itas109/StopBits.cs
${PROJECT_SOURCE_DIR}/../generate/itas109/SerialPortInfo.cs
${PROJECT_SOURCE_DIR}/../generate/cserialportCSharp.cs
${PROJECT_SOURCE_DIR}/../generate/cserialportCSharpPINVOKE.cs
${PROJECT_SOURCE_DIR}/../generate/SerialPortError.cs
${PROJECT_SOURCE_DIR}/../generate/SerialPortInfoVector.cs
)

foreach(src ${ALL_SRCS})
set_source_files_properties(${src} PROPERTIES GENERATED TRUE)
endforeach()

add_executable(${PROJECT_NAME} Program.cs ${ALL_SRCS})