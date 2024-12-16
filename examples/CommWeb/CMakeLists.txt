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

project(cserialport)

# add_definitions(-DCSERIALPORT_DEBUG) # CSerialPort Debug Mode

set(CMAKE_CXX_STANDARD 11)
#add_compile_options(-g)

include_directories(${CMAKE_JS_INC})
include_directories(.)

# Include N-API wrappers
include_directories(node-addon-api)

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

# cserialport nodejs bindings files
set(CSerialPortNodeJSBindRootPath "${CMAKE_CURRENT_SOURCE_DIR}/../../bindings/nodejs")
include_directories(${CSerialPortNodeJSBindRootPath})
include_directories(${CSerialPortNodeJSBindRootPath}/node-addon-api)
list(APPEND CSerialPortNodeJSBindSourceFiles ${CSerialPortNodeJSBindRootPath}/addon.cpp ${CSerialPortNodeJSBindRootPath}/CSerialPortWrapper.cpp)

add_library(${PROJECT_NAME} SHARED ${CSerialPortNodeJSBindSourceFiles} ${CSerialPortSourceFiles} ${CMAKE_JS_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})

if (WIN32)
	target_link_libraries( ${PROJECT_NAME} user32 advapi32 setupapi)
elseif(APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
    target_link_libraries( ${PROJECT_NAME} ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif (UNIX)
	target_link_libraries( ${PROJECT_NAME} pthread)
endif()
