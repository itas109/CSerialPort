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

if (CSERIALPORT_ENABLE_DEBUG)
    add_definitions(-DCSERIALPORT_DEBUG) # CSerialPort Debug Mode
endif ()
add_definitions(-DCSERIALPORT_USE_UTF8) # CSerialPort UTF8 Character Encoding
add_definitions(-DCSERIALPORT_BINDING_LANGUAGE=Python) # CSerialPort Binding Language

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

# find python3
find_package (Python3 COMPONENTS Interpreter Development)
include_directories(${Python3_INCLUDE_DIRS})
link_libraries(${Python3_LIBRARIES})

if (WIN32)
    link_libraries(user32 advapi32 setupapi)
elseif (APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
    link_libraries(${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif (UNIX)
    link_libraries(pthread)
endif ()

# find swig
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

# swig -outdir option(*.py xxx_wrap.cxx xxx_wrap.h)
set(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_SOURCE_DIR}/generate")

# call swig in c++ mode
set_property(SOURCE cserialport.i PROPERTY CPLUSPLUS ON)
# swig -python -c++ -outdir generate -I../../include cserialport.i
swig_add_library(${PROJECT_NAME} LANGUAGE python SOURCES cserialport.i ${CSerialPortSourceFiles})