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
cmake_minimum_required(VERSION 3.10.0)

project(CommC)

include_directories(../) # cserialport.h

if (CSERIALPORT_ENABLE_NATIVE_SYNC)
add_executable(${PROJECT_NAME} main_sync_native.c)
else ()
add_executable(${PROJECT_NAME} main.c)
endif ()

target_link_libraries(${PROJECT_NAME} cserialport)