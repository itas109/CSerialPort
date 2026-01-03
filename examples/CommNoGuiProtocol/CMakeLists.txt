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

project(CSerialPortDemoNoGuiProtocol)

# C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DCSERIALPORT_DEBUG) # CSerialPort Debug Mode
# add_definitions(-DCSERIALPORT_USE_UTF8) # CSerialPort UTF8 Character Encoding

find_package(CSerialPort QUIET)
if (CSerialPort_FOUND)
  add_executable( ${PROJECT_NAME} CSerialPortDemoNoGui.cpp)
  include_directories(${CSerialPort_INCLUDE_DIR})
  target_link_libraries (${PROJECT_NAME} ${CSerialPort_LIBRARY})
else()
  # compile with source code
  message(STATUS "Not found system CSerialPort, compile example with source code")

  # include source files if CSerialPortSourceFiles is empty
  if(NOT CSerialPortSourceFiles)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/cserialport_src.cmake)
  endif()
  
  add_executable( ${PROJECT_NAME} CommNoGuiProtocol.cpp ${CSerialPortHeaderFiles} ${CSerialPortSourceFiles})

  cserialport_target_link_libraries(${PROJECT_NAME})
endif ()