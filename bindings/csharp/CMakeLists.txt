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

project(cserialport)

# set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# cserialport files
set(CSerialPortRootPath "${CMAKE_CURRENT_SOURCE_DIR}/../..")
include_directories(${CSerialPortRootPath}/include)
list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPort.cpp ${CSerialPortRootPath}/src/SerialPortBase.cpp ${CSerialPortRootPath}/src/SerialPortInfo.cpp ${CSerialPortRootPath}/src/SerialPortInfoBase.cpp)
if (WIN32)
    list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortInfoWinBase.cpp ${CSerialPortRootPath}/src/SerialPortWinBase.cpp)
    list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/lib/version.rc)
elseif (UNIX)
    list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortInfoUnixBase.cpp ${CSerialPortRootPath}/src/SerialPortUnixBase.cpp)
endif ()

# find swig
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

# swig -outdir option(*.cs xxx_wrap.cxx xxx_wrap.h)
set(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_SOURCE_DIR}/generate")

# call swig in c++ mode
set_property(SOURCE cserialport.i PROPERTY CPLUSPLUS ON)
# set namespace
set(CMAKE_SWIG_FLAGS -namespace itas109)
# swig -csharp -c++ -outdir generate -I../../include -namespace itas109 cserialport.i
swig_add_library(${PROJECT_NAME} LANGUAGE csharp SOURCES cserialport.i ${CSerialPortSourceFiles})

# example
add_subdirectory(example)