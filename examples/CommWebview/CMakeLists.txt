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

project(CommWebview)

# C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CSERIALPORT_ENABLE_DEBUG)
    add_definitions(-DCSERIALPORT_DEBUG) # CSerialPort Debug Mode
endif ()

if (WIN32)
    # WebView2.h
    # curl -sSL "https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2" | tar -xf - -C 
elseif(APPLE)
    
elseif(UNIX)
    # Find GTK3 and WebKit2GTK
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    pkg_check_modules(WEBKIT2GTK REQUIRED webkit2gtk-4.0)

    include_directories(${GTK3_INCLUDE_DIRS} ${WEBKIT2GTK_INCLUDE_DIRS})
    link_directories(${GTK3_LIBRARY_DIRS} ${WEBKIT2GTK_LIBRARY_DIRS})
endif()

include_directories(.)

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
if (CSERIALPORT_ENABLE_NATIVE_SYNC)
    # TODO
else()
    list(APPEND CSerialPortHeaderFiles ${CSerialPortRootPath}/include/CSerialPort/SerialPortAsyncBase.h)
    list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortAsyncBase.cpp)
endif ()

add_executable(${PROJECT_NAME} main.cpp ${CSerialPortSourceFiles})

if (WIN32)
	target_link_libraries( ${PROJECT_NAME} user32 advapi32 setupapi)
elseif(APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
    find_library(WebKit_LIBRARY WebKit REQUIRED)
    target_link_libraries( ${PROJECT_NAME} ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY} ${WebKit_LIBRARY})
elseif (UNIX)
	target_link_libraries( ${PROJECT_NAME} pthread ${GTK3_LIBRARIES} ${WEBKIT2GTK_LIBRARIES})
endif()
