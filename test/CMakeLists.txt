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

project(CSerialPortTest)

if (CSERIALPORT_ENABLE_DEBUG)
    add_definitions(-DCSERIALPORT_DEBUG) # CSerialPort Debug Mode
endif ()
# add_definitions(-DCSERIALPORT_USE_UTF8) # CSerialPort UTF8 Character Encoding

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

# C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# include source files if CSerialPortSourceFiles is empty
if(NOT CSerialPortSourceFiles)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/cserialport_src.cmake)
endif()

include_directories(.)

# virtual serial port source code
include_directories(.)
list(APPEND CSerialPortVirtualSourceFiles CSerialPortVirtual.cpp)

add_executable(${PROJECT_NAME} cseiralport_test.cpp ${CSerialPortHeaderFiles} ${CSerialPortSourceFiles} ${CSerialPortVirtualSourceFiles})

cserialport_target_link_libraries(${PROJECT_NAME})
if(UNIX) # Linux or MacOS or BSD etc.
    target_link_libraries( ${PROJECT_NAME} util) # pty
endif ()