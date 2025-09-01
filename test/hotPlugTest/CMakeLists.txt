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

cmake_minimum_required(VERSION 3.25)

project(CSerialPortHotPlug)

set(CSerialPortRootPath "${CMAKE_CURRENT_SOURCE_DIR}/../..")
include_directories(${CSerialPortRootPath}/include)

add_executable( ${PROJECT_NAME} CSerialPortHotPlugTest.cpp )

if (WIN32)
    target_link_libraries( ${PROJECT_NAME} user32)
elseif(APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
    target_link_libraries( ${PROJECT_NAME} ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif(UNIX)
    target_link_libraries( ${PROJECT_NAME} pthread)
endif ()
