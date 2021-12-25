cmake_minimum_required(VERSION 2.8.12)

project(CommMFC)

find_package(MFC)
if (NOT MFC_FOUND)
  MESSAGE(FATAL_ERROR "MFC not found")
endif()

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2) # 1 the static MFC library 2 shared MFC library

# add by itas109
set(CSerialPortRootPath "${CMAKE_CURRENT_SOURCE_DIR}/../../")
include_directories(${CSerialPortRootPath}/include)
list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPort.cpp ${CSerialPortRootPath}/src/SerialPortBase.cpp ${CSerialPortRootPath}/src/SerialPortInfo.cpp ${CSerialPortRootPath}/src/SerialPortInfoBase.cpp)
if(WIN32)
    list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortInfoWinBase.cpp ${CSerialPortRootPath}/src/SerialPortWinBase.cpp)
elseif(UNIX)
    list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortInfoUnixBase.cpp ${CSerialPortRootPath}/src/SerialPortUnixBase.cpp)
endif()
# end by itas109

set(MFCFiles CommMFC.cpp CommMFC.h CommMFC.rc CommMFCDlg.cpp CommMFCDlg.h Resource.h stdafx.cpp stdafx.h targetver.h)

add_executable(${PROJECT_NAME} WIN32 ${MFCFiles} ${CSerialPortSourceFiles})

# add by itas109
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
# end by itas109