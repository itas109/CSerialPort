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

project(CSerialPort LANGUAGES CXX)

# set output directory
if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

# set build tpye
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(CSERIALPORT_BUILD_EXAMPLES "Build CSerialPort examples" ON)
option(CSERIALPORT_BUILD_DOC "Build CSerialPort Doc" OFF)
option(CSERIALPORT_BUILD_TEST "Build CSerialPort Test" OFF)

# only build library if add CSeiralPort as subdirectory
if (NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(CSERIALPORT_BUILD_EXAMPLES OFF)
    set(CSERIALPORT_BUILD_DOC OFF)
    set(CSERIALPORT_BUILD_TEST OFF)
endif()

message(STATUS "CSerialPort CMake Info")
message(STATUS "=======================================================")
message(STATUS "    Operation System :  ${CMAKE_SYSTEM}")
message(STATUS "    CPU Architecture : ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "    Build Type : ${CMAKE_BUILD_TYPE}")
message(STATUS "    Shared Library  : ${BUILD_SHARED_LIBS}")
message(STATUS "    Build Examples  : ${CSERIALPORT_BUILD_EXAMPLES}")
message(STATUS "    Build Doc  : ${CSERIALPORT_BUILD_DOC}")
message(STATUS "    Build Test  : ${CSERIALPORT_BUILD_TEST}")
message(STATUS "=======================================================")

# libcserialport
add_subdirectory(lib)

# examples
if (CSERIALPORT_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif ()

# test
if (CSERIALPORT_BUILD_TEST)
	add_subdirectory(test)
endif ()

# doc
if (CSERIALPORT_BUILD_DOC)
	add_subdirectory(doc)
endif ()