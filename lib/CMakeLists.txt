cmake_minimum_required(VERSION 2.8.12)

set(LIB_NAME libcserialport)

if(APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
endif()

if (NOT BUILD_SHARED_LIBS)
	set(BUILD_SHARED_LIBS ON)
endif()

# static libarary
# add_compile_options(-static)

set(CSerialPortRootPath "${CMAKE_CURRENT_SOURCE_DIR}/..")
include_directories(${CSerialPortRootPath}/include)
list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPort.cpp ${CSerialPortRootPath}/src/SerialPortBase.cpp ${CSerialPortRootPath}/src/SerialPortInfo.cpp ${CSerialPortRootPath}/src/SerialPortInfoBase.cpp)
if (WIN32)
    list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortInfoWinBase.cpp ${CSerialPortRootPath}/src/SerialPortWinBase.cpp)
elseif (UNIX)
    list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortInfoUnixBase.cpp ${CSerialPortRootPath}/src/SerialPortUnixBase.cpp)
endif ()

add_library( ${LIB_NAME} ${CSerialPortSourceFiles})

# remove prefix
set_target_properties( ${LIB_NAME} PROPERTIES PREFIX "")

# preprocessor definitions for compiling CSerialPort library
set_target_properties( ${LIB_NAME} PROPERTIES COMPILE_DEFINITIONS BUILDING_LIBCSERIALPORT)

if (WIN32)
	# for function availableFriendlyPorts
	target_link_libraries( ${LIB_NAME} setupapi)
	#target_link_libraries( ${LIB_NAME} libgcc.a)
	#target_link_libraries( ${LIB_NAME} libstdc++.a)
	#target_link_libraries( ${LIB_NAME} libpthread.a)
elseif(APPLE)
    target_link_libraries( ${LIB_NAME} ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif (UNIX)
	target_link_libraries( ${LIB_NAME} pthread)
endif()

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/CSerialPort DESTINATION include FILES_MATCHING PATTERN "*.h")

install(TARGETS libcserialport
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		)

install(FILES ${PROJECT_SOURCE_DIR}/cserialport-config.cmake.in DESTINATION lib/cmake/CSerialPort RENAME cserialport-config.cmake)