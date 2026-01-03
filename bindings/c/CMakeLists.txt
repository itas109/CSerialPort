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

project(cserialport)

# C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CSERIALPORT_ENABLE_DEBUG)
    add_definitions(-DCSERIALPORT_DEBUG) # CSerialPort Debug Mode
endif ()
if (CSERIALPORT_ENABLE_UTF8)
    add_definitions(-DCSERIALPORT_USE_UTF8) # CSerialPort UTF8 Character Encoding
endif ()
add_definitions(-DCSERIALPORT_BINDING_LANGUAGE=C) # CSerialPort Binding Language

# set output directory
if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif ()
if (NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif ()
if (NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif ()

# cserialport files
# include source files if CSerialPortSourceFiles is empty
if(NOT CSerialPortSourceFiles)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/cserialport_src.cmake)
endif()

add_library(${PROJECT_NAME} SHARED cserialport.cpp ${CSerialPortHeaderFiles} ${CSerialPortSourceFiles})

cserialport_target_link_libraries(${PROJECT_NAME})

# example
add_subdirectory(example)