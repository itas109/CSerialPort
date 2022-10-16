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

project(CommWXWidgets)

# set(wxWidgets_ROOT_DIR "D:/wxWidgets") # wxWidgets 3.1.7
find_package(wxWidgets COMPONENTS richtext core base REQUIRED)
if (NOT wxWidgets_FOUND)
    message(FATAL_ERROR "wxWidgets not found. sudo apt install libwxgtk3.0-gtk3-dev")
endif()
include(${wxWidgets_USE_FILE})

set(CSerialPortRootPath "${CMAKE_CURRENT_SOURCE_DIR}/../../")
include_directories(${CSerialPortRootPath}/include)
list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPort.cpp ${CSerialPortRootPath}/src/SerialPortBase.cpp ${CSerialPortRootPath}/src/SerialPortInfo.cpp ${CSerialPortRootPath}/src/SerialPortInfoBase.cpp)
if(WIN32)
    list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortInfoWinBase.cpp ${CSerialPortRootPath}/src/SerialPortWinBase.cpp)
elseif(UNIX)
    list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortInfoUnixBase.cpp ${CSerialPortRootPath}/src/SerialPortUnixBase.cpp)
endif()

set(wxWidgetsFiles CommWXWidgetsMain.cpp CommWXWidgetsMain.h CommWXWidgetsApp.cpp CommWXWidgetsApp.h resource.rc)

add_executable(${PROJECT_NAME} ${wxWidgetsFiles} ${CSerialPortSourceFiles})

target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

if (WIN32)
    # for function availableFriendlyPorts
    target_link_libraries( ${PROJECT_NAME} setupapi)
elseif(APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
    target_link_libraries( ${PROJECT_NAME} ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif(UNIX)
    target_link_libraries( ${PROJECT_NAME} pthread)
endif ()