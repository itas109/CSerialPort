project(CSerialPortDemoNoGui)

cmake_minimum_required(VERSION 2.8)

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

include_directories(../../src)

file(GLOB_RECURSE COMMON_SOURCES ../../src/SerialPort.cpp ../../src/SerialPortBase.cpp ../../src/SerialPortInfo.cpp ../../src/SerialPortInfoBase.cpp)

IF (CMAKE_HOST_WIN32)
	file(GLOB_RECURSE OS_ABOUT_SOURCES ../../src/SerialPortInfoWinBase.cpp ../../src/SerialPortWinBase.cpp)
ELSEIF (CMAKE_HOST_UNIX)
	file(GLOB_RECURSE OS_ABOUT_SOURCES ../../src/SerialPortInfoUnixBase.cpp ../../src/SerialPortUnixBase.cpp)
ENDIF ()

add_executable(CSerialPortDemoNoGui CSerialPortDemoNoGui.cpp ${COMMON_SOURCES} ${OS_ABOUT_SOURCES})

IF (CMAKE_HOST_WIN32)
	# for function availableFriendlyPorts
	target_link_libraries( CSerialPortDemoNoGui setupapi )
ELSEIF (CMAKE_HOST_UNIX)
	target_link_libraries( CSerialPortDemoNoGui pthread )
ENDIF ()