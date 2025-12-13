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

project(CSerialPort LANGUAGES CXX)

# C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set output directory
if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    endif ()
    if (NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    endif ()
    if (NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    endif ()
endif()

# set build type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(CSERIALPORT_ENABLE_NATIVE_SYNC "Enable CSerialPort native synchronous mode" OFF)
option(CSERIALPORT_BUILD_EXAMPLES "Build CSerialPort examples" ON)
option(CSERIALPORT_BUILD_BINDING_C "Build CSerialPort c binding" ON)
option(CSERIALPORT_ENABLE_DEBUG "Enable CSerialPort Debug Log" OFF)
option(CSERIALPORT_ENABLE_UTF8 "Enable CSerialPort UTF8" OFF)
option(CSERIALPORT_BUILD_DOC "Build CSerialPort Doc" OFF)
option(CSERIALPORT_BUILD_TEST "Build CSerialPort Test" OFF)

# only build library if add CSerialPort as subdirectory
if (NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(CSERIALPORT_BUILD_EXAMPLES OFF)
    set(CSERIALPORT_BUILD_BINDING_C OFF)
    set(CSERIALPORT_BUILD_DOC OFF)
    set(CSERIALPORT_BUILD_TEST OFF)
endif()

message(STATUS "CSerialPort CMake Info")
message(STATUS "=======================================================")
message(STATUS "    Operation System : ${CMAKE_SYSTEM}")
message(STATUS "    CPU Architecture : ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "    Build Type : ${CMAKE_BUILD_TYPE}")
message(STATUS "    Shared Library [ BUILD_SHARED_LIBS ] : ${BUILD_SHARED_LIBS}")
message(STATUS "    Enable Native Sync Mode [ CSERIALPORT_ENABLE_NATIVE_SYNC ] : ${CSERIALPORT_ENABLE_NATIVE_SYNC}")
message(STATUS "    Build Examples [ CSERIALPORT_BUILD_EXAMPLES ] : ${CSERIALPORT_BUILD_EXAMPLES}")
message(STATUS "    Build c binding [ CSERIALPORT_BUILD_BINDING_C ] : ${CSERIALPORT_BUILD_BINDING_C}")
message(STATUS "    Enable Debug [ CSERIALPORT_ENABLE_DEBUG ] : ${CSERIALPORT_ENABLE_DEBUG}")
message(STATUS "    Enable UTF8 [ CSERIALPORT_ENABLE_UTF8 ] : ${CSERIALPORT_ENABLE_UTF8}")
message(STATUS "    Build Doc [ CSERIALPORT_BUILD_DOC ] : ${CSERIALPORT_BUILD_DOC}")
message(STATUS "    Build Test [ CSERIALPORT_BUILD_TEST ] : ${CSERIALPORT_BUILD_TEST}")
message(STATUS "=======================================================")

if (CSERIALPORT_ENABLE_NATIVE_SYNC)
    add_definitions(-DCSERIALPORT_NATIVE_SYNC) # CSerialPort Native Sync Mode
endif ()

# common headers and source files
include(cmake/cserialport_src.cmake)

# libcserialport
add_subdirectory(lib)

# examples
if (CSERIALPORT_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

# bindings/c
if (CSERIALPORT_BUILD_BINDING_C)
    add_subdirectory(bindings/c)
endif ()

# test
if (CSERIALPORT_BUILD_TEST)
    add_subdirectory(test)
endif ()

# doc
if (CSERIALPORT_BUILD_DOC)
    add_subdirectory(doc)
endif ()