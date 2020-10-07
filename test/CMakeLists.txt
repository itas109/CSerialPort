project(CSerialPortTest)

cmake_minimum_required(VERSION 2.8)

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

add_custom_target(get_gtest
		COMMAND git submodule update --init --recursive
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	)

include(gtest.cmake)

add_dependencies(gtest get_gtest)

include_directories(.)
include_directories(../include)

#link_directories(${PROJECT_SOURCE_DIR}/lib )

file(GLOB_RECURSE COMMON_SOURCES ../src/SerialPort.cpp ../src/SerialPortBase.cpp ../src/SerialPortInfo.cpp ../src/SerialPortInfoBase.cpp)

IF (CMAKE_HOST_WIN32)
	file(GLOB_RECURSE OS_ABOUT_SOURCES ../src/SerialPortInfoWinBase.cpp ../src/SerialPortWinBase.cpp)
ELSEIF (CMAKE_HOST_UNIX)
	file(GLOB_RECURSE OS_ABOUT_SOURCES ../src/SerialPortInfoUnixBase.cpp ../src/SerialPortUnixBase.cpp)
ENDIF ()

add_executable(CSerialPortTest cseiralport_test.cpp ${COMMON_SOURCES} ${OS_ABOUT_SOURCES})

#target_link_libraries( CSerialPortTest gtest gtest_main)
target_link_libraries( CSerialPortTest gtest)

IF (CMAKE_HOST_WIN32)
	# for function availableFriendlyPorts
	target_link_libraries( CSerialPortTest setupapi )
ELSEIF (CMAKE_HOST_UNIX)
	target_link_libraries( CSerialPortTest pthread )
ENDIF ()