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

add_definitions(-DCSERIALPORT_DEBUG) # CSerialPort Debug Mode
# add_definitions(-DCSERIALPORT_USE_UTF8) # CSerialPort UTF8 Character Encoding
add_definitions(-DCSERIALPORT_BINDING_LANGUAGE=C) # CSerialPort Binding Language

# set output directory
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)					
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)					
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)					
endif()

# cserialport files
set(CSerialPortRootPath "${CMAKE_CURRENT_SOURCE_DIR}/../..")
list(APPEND CSerialPortSourceFiles 
    ${CSerialPortRootPath}/src/SerialPort.cpp 
    ${CSerialPortRootPath}/src/SerialPortBase.cpp 
    ${CSerialPortRootPath}/src/SerialPortInfo.cpp 
    ${CSerialPortRootPath}/src/SerialPortInfoBase.cpp
)
if (WIN32)
    list(APPEND CSerialPortSourceFiles 
        ${CSerialPortRootPath}/src/SerialPortInfoWinBase.cpp 
        ${CSerialPortRootPath}/src/SerialPortWinBase.cpp
    )
    list(APPEND CSerialPortSourceFiles 
        ${CSerialPortRootPath}/lib/version.rc
    )
elseif (UNIX)
    list(APPEND CSerialPortSourceFiles 
        ${CSerialPortRootPath}/src/SerialPortInfoUnixBase.cpp 
        ${CSerialPortRootPath}/src/SerialPortUnixBase.cpp
    )
endif ()

add_library(${PROJECT_NAME} SHARED cserialport.cpp ${CSerialPortSourceFiles})
target_include_directories(cserialport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(cserialport PRIVATE ${CSerialPortRootPath}/include)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} user32 advapi32 setupapi)
elseif (APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
    target_link_libraries(${PROJECT_NAME} ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} pthread)
    if(CMAKE_COMPILER_IS_GNUCC)
        target_link_libraries(${PROJECT_NAME} rt)
    endif()
endif ()

# example
if (CSERIALPORT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif ()
