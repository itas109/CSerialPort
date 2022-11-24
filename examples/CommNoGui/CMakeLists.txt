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

project(CSerialPortDemoNoGui)

add_definitions(-DCSERIALPORT_DEBUG) # CSerialPort Debug Mode

find_package(CSerialPort QUIET)
if (CSerialPort_FOUND)
  add_executable( ${PROJECT_NAME} CSerialPortDemoNoGui.cpp)
  include_directories(${CSerialPort_INCLUDE_DIR})
  target_link_libraries (${PROJECT_NAME} ${CSerialPort_LIBRARY})
else()
  # compile with source code
  message(STATUS "Not found system CSerialPort, compile example with source code")

  set(CSerialPortRootPath "${CMAKE_CURRENT_SOURCE_DIR}/../..")
  include_directories(${CSerialPortRootPath}/include)
  list(APPEND CSerialPortHeaderFiles 
    ${CSerialPortRootPath}/include/CSerialPort/SerialPort_global.h
    ${CSerialPortRootPath}/include/CSerialPort/SerialPort_version.h
    ${CSerialPortRootPath}/include/CSerialPort/SerialPort.h
    ${CSerialPortRootPath}/include/CSerialPort/SerialPortInfo.h
    ${CSerialPortRootPath}/include/CSerialPort/SerialPortBase.h
    ${CSerialPortRootPath}/include/CSerialPort/SerialPortInfoBase.h
    ${CSerialPortRootPath}/include/CSerialPort/SerialPortListener.h
    ${CSerialPortRootPath}/include/CSerialPort/ibuffer.hpp
    ${CSerialPortRootPath}/include/CSerialPort/ithread.hpp
    ${CSerialPortRootPath}/include/CSerialPort/itimer.hpp
    ${CSerialPortRootPath}/include/CSerialPort/iutils.hpp
  )
  list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPort.cpp ${CSerialPortRootPath}/src/SerialPortBase.cpp ${CSerialPortRootPath}/src/SerialPortInfo.cpp ${CSerialPortRootPath}/src/SerialPortInfoBase.cpp)
  if (WIN32)
      list(APPEND CSerialPortHeaderFiles ${CSerialPortRootPath}/include/CSerialPort/SerialPortWinBase.h ${CSerialPortRootPath}/include/CSerialPort/SerialPortInfoWinBase.h)
      list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortInfoWinBase.cpp ${CSerialPortRootPath}/src/SerialPortWinBase.cpp)
  elseif (UNIX)
      list(APPEND CSerialPortHeaderFiles ${CSerialPortRootPath}/include/CSerialPort/SerialPortUnixBase.h ${CSerialPortRootPath}/include/CSerialPort/SerialPortInfoUnixBase.h)
      list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortInfoUnixBase.cpp ${CSerialPortRootPath}/src/SerialPortUnixBase.cpp)
  endif ()

  add_executable( ${PROJECT_NAME} CSerialPortDemoNoGui.cpp ${CSerialPortHeaderFiles} ${CSerialPortSourceFiles})

  if (WIN32)
	# for function availableFriendlyPorts
	target_link_libraries( ${PROJECT_NAME} setupapi)
  elseif(APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
    target_link_libraries( ${PROJECT_NAME} ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
  elseif(UNIX)
	target_link_libraries( ${PROJECT_NAME} pthread)
  endif ()
endif ()