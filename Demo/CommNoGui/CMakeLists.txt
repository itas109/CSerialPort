project(CSerialPortDemoNoGui)

cmake_minimum_required(VERSION 2.8)

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

include_directories(../../src)

IF (NOT CMAKE_HOST_APPLE)
aux_source_directory(. DIR_SRCS)
ENDIF ()
aux_source_directory(../../src DIR_SRCS)

IF (CMAKE_HOST_WIN32)
	list(REMOVE_ITEM DIR_SRCS ../../src/SerialPortInfoUnixBase.cpp)
	list(REMOVE_ITEM DIR_SRCS ../../src/SerialPortUnixBase.cpp)
ELSEIF (CMAKE_HOST_UNIX)
	list(REMOVE_ITEM DIR_SRCS ../../src/SerialPortInfoWinBase.cpp)
	list(REMOVE_ITEM DIR_SRCS ../../src/SerialPortWinBase.cpp)
ENDIF ()

add_executable(CSerialPortDemoNoGui ${DIR_SRCS})

IF (CMAKE_HOST_WIN32)
	# for function availableFriendlyPorts
	target_link_libraries( CSerialPortDemoNoGui setupapi )
ELSEIF (CMAKE_HOST_UNIX)
	target_link_libraries( CSerialPortDemoNoGui pthread )
ENDIF ()