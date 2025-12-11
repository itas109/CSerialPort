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
set(CSerialPortRootPath "${CMAKE_CURRENT_SOURCE_DIR}/../..")
include_directories(${CSerialPortRootPath}/include)
list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPort.cpp ${CSerialPortRootPath}/src/SerialPortBase.cpp ${CSerialPortRootPath}/src/SerialPortInfo.cpp ${CSerialPortRootPath}/src/SerialPortInfoBase.cpp)
if (CSERIALPORT_ENABLE_NATIVE_SYNC)
    if (WIN32)
        list(APPEND CSerialPortHeaderFiles ${CSerialPortRootPath}/include/CSerialPort/SerialPortNativeSyncWinBase.h ${CSerialPortRootPath}/include/CSerialPort/SerialPortInfoWinBase.h)
        list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortNativeSyncWinBase.cpp ${CSerialPortRootPath}/src/SerialPortInfoWinBase.cpp)
        list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/lib/version.rc)
    elseif (UNIX)
        list(APPEND CSerialPortHeaderFiles ${CSerialPortRootPath}/include/CSerialPort/SerialPortNativeSyncUnixBase.h ${CSerialPortRootPath}/include/CSerialPort/SerialPortInfoUnixBase.h)
        list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortNativeSyncUnixBase.cpp ${CSerialPortRootPath}/src/SerialPortInfoUnixBase.cpp)
    endif ()
else()
    list(APPEND CSerialPortHeaderFiles ${CSerialPortRootPath}/include/CSerialPort/SerialPortAsyncBase.h)
    list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortAsyncBase.cpp)
    if (WIN32)
        list(APPEND CSerialPortHeaderFiles ${CSerialPortRootPath}/include/CSerialPort/SerialPortWinBase.h ${CSerialPortRootPath}/include/CSerialPort/SerialPortInfoWinBase.h)
        list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortWinBase.cpp ${CSerialPortRootPath}/src/SerialPortInfoWinBase.cpp)
        list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/lib/version.rc)
    elseif (UNIX)
        list(APPEND CSerialPortHeaderFiles ${CSerialPortRootPath}/include/CSerialPort/SerialPortUnixBase.h ${CSerialPortRootPath}/include/CSerialPort/SerialPortInfoUnixBase.h)
        list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortUnixBase.cpp ${CSerialPortRootPath}/src/SerialPortInfoUnixBase.cpp)
    endif ()
endif ()

add_library(${PROJECT_NAME} SHARED cserialport.cpp ${CSerialPortSourceFiles})

if (WIN32)
    target_link_libraries(${PROJECT_NAME} user32 advapi32 setupapi)
elseif (APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
    target_link_libraries(${PROJECT_NAME} ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} pthread)
endif ()

# example
add_subdirectory(example)