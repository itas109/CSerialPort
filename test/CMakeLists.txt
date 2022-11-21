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

project(CSerialPortTest)

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

add_custom_target(get_gtest
		COMMAND git submodule update --init --recursive
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	)

include(gtest.cmake)

add_dependencies(gtest get_gtest)

include_directories(.)

set(CSerialPortRootPath "${CMAKE_CURRENT_SOURCE_DIR}/../")
include_directories(${CSerialPortRootPath}/include)
list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPort.cpp ${CSerialPortRootPath}/src/SerialPortBase.cpp ${CSerialPortRootPath}/src/SerialPortInfo.cpp ${CSerialPortRootPath}/src/SerialPortInfoBase.cpp)
if(WIN32)
    list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortInfoWinBase.cpp ${CSerialPortRootPath}/src/SerialPortWinBase.cpp)
elseif(UNIX)
    list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortInfoUnixBase.cpp ${CSerialPortRootPath}/src/SerialPortUnixBase.cpp)
endif()

# virtual serial port source code
include_directories(.)
list(APPEND CSerialPortVirtualSourceFiles CSerialPortVirtual.cpp)

add_executable(${PROJECT_NAME} cseiralport_test.cpp ${CSerialPortSourceFiles} ${CSerialPortVirtualSourceFiles})
target_link_libraries( ${PROJECT_NAME} gtest)

if (WIN32)
    # for function availableFriendlyPorts
    target_link_libraries( ${PROJECT_NAME} setupapi)
elseif(APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
    target_link_libraries( ${PROJECT_NAME} ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif(UNIX)
    target_link_libraries( ${PROJECT_NAME} pthread)
    target_link_libraries( ${PROJECT_NAME} util) # pty
endif ()