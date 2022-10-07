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

project(CommMFCDLL)

find_package(MFC)
if (NOT MFC_FOUND)
  MESSAGE(FATAL_ERROR "MFC not found")
endif()

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2) # 1 the static MFC library 2 shared MFC library

set(MFCFiles CommMFCDLL.cpp CommMFCDLL.h CommMFCDLL.rc CommMFCDLLDlg.cpp CommMFCDLLDlg.h Resource.h stdafx.cpp stdafx.h targetver.h)

find_package(CSerialPort)
if (CSerialPort_FOUND)
  add_executable(${PROJECT_NAME} WIN32 ${MFCFiles} ${CSerialPortSourceFiles})
  include_directories(${CSerialPort_INCLUDE_DIR})
  target_link_libraries (${PROJECT_NAME} ${CSerialPort_LIBRARY})
else()
  message(FATAL_ERROR "Not found CSerialPort, please compile CSerialPort library first or set CMAKE_PREFIX_PATH to CSerialPort install Directory")
endif ()