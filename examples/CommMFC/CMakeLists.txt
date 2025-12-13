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

project(CommMFC)

# C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CSERIALPORT_ENABLE_DEBUG)
    add_definitions(-DCSERIALPORT_DEBUG) # CSerialPort Debug Mode
endif ()

find_package(MFC)
if (NOT MFC_FOUND)
  MESSAGE(FATAL_ERROR "MFC not found")
endif()

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2) # 1 the static MFC library 2 shared MFC library

# include source files if CSerialPortSourceFiles is empty
if(NOT CSerialPortSourceFiles)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/cserialport_src.cmake)
endif()

set(MFCFiles CommMFC.cpp CommMFC.h CommMFC.rc CommMFCDlg.cpp CommMFCDlg.h Resource.h stdafx.cpp stdafx.h targetver.h)

add_executable(${PROJECT_NAME} WIN32 ${MFCFiles} ${CSerialPortHeaderFiles} ${CSerialPortSourceFiles})

if (WIN32)
    # for function availableFriendlyPorts
    target_link_libraries( ${PROJECT_NAME} user32 advapi32 setupapi)
elseif(APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
    target_link_libraries( ${PROJECT_NAME} ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif(UNIX)
    target_link_libraries( ${PROJECT_NAME} pthread)
endif ()