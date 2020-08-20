cmake_minimum_required(VERSION 2.8)

set(APP_NAME CSerialPortDemoNoGui)

if(APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
endif()

include_directories(../../src)

file(GLOB_RECURSE COMMON_SOURCES ../../src/SerialPort.cpp ../../src/SerialPortBase.cpp ../../src/SerialPortInfo.cpp ../../src/SerialPortInfoBase.cpp)

IF (CMAKE_HOST_WIN32)
	file(GLOB_RECURSE OS_ABOUT_SOURCES ../../src/SerialPortInfoWinBase.cpp ../../src/SerialPortWinBase.cpp)
ELSEIF (CMAKE_HOST_UNIX)
	file(GLOB_RECURSE OS_ABOUT_SOURCES ../../src/SerialPortInfoUnixBase.cpp ../../src/SerialPortUnixBase.cpp)
ENDIF ()

add_executable( ${APP_NAME} CSerialPortDemoNoGui.cpp ${COMMON_SOURCES} ${OS_ABOUT_SOURCES})

IF (CMAKE_HOST_WIN32)
	# for function availableFriendlyPorts
	target_link_libraries( ${APP_NAME} setupapi )
elseif(APPLE)
    target_link_libraries( ${APP_NAME} ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif(UNIX)
	target_link_libraries( ${APP_NAME} pthread )
ENDIF ()