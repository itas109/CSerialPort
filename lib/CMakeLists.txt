cmake_minimum_required(VERSION 2.8)

set(LIB_NAME libcserialport)

include_directories(../src)

file(GLOB_RECURSE COMMON_SOURCES ../src/SerialPort.cpp ../src/SerialPortBase.cpp ../src/SerialPortInfo.cpp ../src/SerialPortInfoBase.cpp)

if (CMAKE_HOST_WIN32)
	file(GLOB_RECURSE OS_ABOUT_SOURCES ../src/SerialPortInfoWinBase.cpp ../src/SerialPortWinBase.cpp)
	if(MSVC)
  		list(APPEND OS_ABOUT_SOURCES libcserialport.rc)
	endif()
elseif (CMAKE_HOST_UNIX)
	file(GLOB_RECURSE OS_ABOUT_SOURCES ../src/SerialPortInfoUnixBase.cpp ../src/SerialPortUnixBase.cpp)
endif ()

add_library( ${LIB_NAME} ${COMMON_SOURCES} ${OS_ABOUT_SOURCES})

# remove prefix
set_target_properties( ${LIB_NAME} PROPERTIES PREFIX "")

if (CMAKE_HOST_WIN32)
	# for function availableFriendlyPorts
	target_link_libraries( ${LIB_NAME} setupapi )
elseif (CMAKE_HOST_UNIX)
	target_link_libraries( ${LIB_NAME} pthread )
endif()