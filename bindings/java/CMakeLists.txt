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
cmake_minimum_required(VERSION 3.8.2)

project(cserialport)

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

# find java
find_package(Java REQUIRED)
find_package(JNI REQUIRED)
include(UseJava) # add_jar
include_directories(${JNI_INCLUDE_DIRS}) # jni.h

# find swig
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

# swig -outdir option(*.java xxx_wrap.cxx xxx_wrap.h)
set(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_SOURCE_DIR}/generate")

# call swig in c++ mode
set_property(SOURCE cserialport.i PROPERTY CPLUSPLUS ON)
# set swig package
set(CMAKE_SWIG_FLAGS -package com.itas109.cserialport)
# swig -java -c++ -outdir generate -I../../include -package com.itas109.cserialport cserialport.i
swig_add_library(${PROJECT_NAME} LANGUAGE java SOURCES cserialport.i ${CSerialPortSourceFiles})

# example
set(EXAMPLE_NAME CommJava) 
add_jar(
    ${EXAMPLE_NAME} 
    example/CommJava.java
    ${CMAKE_SWIG_OUTDIR}/cserialportJava.java
    ${CMAKE_SWIG_OUTDIR}/cserialportJavaJNI.java
    ${CMAKE_SWIG_OUTDIR}/CSerialPort.java
    ${CMAKE_SWIG_OUTDIR}/CSerialPortListener.java
    ${CMAKE_SWIG_OUTDIR}/CSerialPortInfo.java
    ${CMAKE_SWIG_OUTDIR}/BaudRate.java
    ${CMAKE_SWIG_OUTDIR}/DataBits.java
    ${CMAKE_SWIG_OUTDIR}/FlowControl.java
    ${CMAKE_SWIG_OUTDIR}/OperateMode.java
    ${CMAKE_SWIG_OUTDIR}/Parity.java
    ${CMAKE_SWIG_OUTDIR}/StopBits.java
    ${CMAKE_SWIG_OUTDIR}/SerialPortError.java
    ${CMAKE_SWIG_OUTDIR}/SerialPortInfo.java
    ${CMAKE_SWIG_OUTDIR}/SerialPortInfoVector.java
    ENTRY_POINT CommJava # CommJava.java main class name
)
add_dependencies( ${EXAMPLE_NAME} ${PROJECT_NAME} )

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}
    COMMENT "copy cserialport library"
    VERBATIM
)