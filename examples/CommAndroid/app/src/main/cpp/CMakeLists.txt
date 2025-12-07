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

project("cserialport")

# C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CSERIALPORT_ENABLE_DEBUG)
    add_definitions(-DCSERIALPORT_DEBUG) # CSerialPort Debug Mode
endif ()
add_definitions(-DCSERIALPORT_USE_UTF8) # CSerialPort UTF8 Character Encoding
add_definitions(-DCSERIALPORT_BINDING_LANGUAGE=Android) # CSerialPort Binding Language

# cserialport files
set(CSerialPortRootPath "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../")
include_directories(${CSerialPortRootPath}/include)
list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPort.cpp ${CSerialPortRootPath}/src/SerialPortBase.cpp ${CSerialPortRootPath}/src/SerialPortInfo.cpp ${CSerialPortRootPath}/src/SerialPortInfoBase.cpp)
# android
list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortInfoUnixBase.cpp ${CSerialPortRootPath}/src/SerialPortUnixBase.cpp)
if (CSERIALPORT_ENABLE_NATIVE_SYNC)
    # TODO
else()
    list(APPEND CSerialPortHeaderFiles ${CSerialPortRootPath}/include/CSerialPort/SerialPortAsyncBase.h)
    list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortAsyncBase.cpp)
endif ()

# find java
find_package(Java REQUIRED)
#find_package(JNI REQUIRED)
include(UseJava) # add_jar
include_directories(${JNI_INCLUDE_DIRS}) # jni.h

# The android libc, bionic, provides built-in support for pthreads, so no additional linking (-lpthreads) is necessary.
# link_libraries(pthread)

# find swig
# set(SWIG_EXECUTABLE "D:/swigwin-4.3.0/swig.exe")
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

# swig -outdir option(*.java xxx_wrap.cxx xxx_wrap.h)
set(JAVA_GEN_PACKAGE "com.itas109.cserialport.jni")
string(REPLACE "." "/" JAVA_GEN_SUBDIR ${JAVA_GEN_PACKAGE})
set(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_SOURCE_DIR}/../java/${JAVA_GEN_SUBDIR}")

# call swig in c++ mode
set_property(SOURCE cserialport.i PROPERTY CPLUSPLUS ON)
# set swig package
set(CMAKE_SWIG_FLAGS -package ${JAVA_GEN_PACKAGE})
# swig -java -c++ -outdir generate -I../../include -package com.itas109.cserialport.jni cserialport.i
swig_add_library(${PROJECT_NAME} LANGUAGE java SOURCES cserialport.i ${CSerialPortSourceFiles})

find_library( log-lib log )
target_link_libraries( ${PROJECT_NAME} ${log-lib} )