project(libcserialport)

cmake_minimum_required(VERSION 2.8)

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

include_directories(../src)

aux_source_directory(../src DIR_SRCS)

IF (CMAKE_HOST_WIN32)
	list(REMOVE_ITEM DIR_SRCS ../src/SerialPortInfoUnixBase.cpp)
	list(REMOVE_ITEM DIR_SRCS ../src/SerialPortUnixBase.cpp)
ELSEIF (CMAKE_HOST_UNIX)
	list(REMOVE_ITEM DIR_SRCS ../src/SerialPortInfoWinBase.cpp)
	list(REMOVE_ITEM DIR_SRCS ../src/SerialPortWinBase.cpp)
ENDIF ()
add_library( cserialport SHARED ${DIR_SRCS})

IF (CMAKE_HOST_WIN32)
	# for function availableFriendlyPorts
	target_link_libraries( cserialport setupapi )
ELSEIF (CMAKE_HOST_UNIX)
	target_link_libraries( cserialport pthread )
ENDIF ()