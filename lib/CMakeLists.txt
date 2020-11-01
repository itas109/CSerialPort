cmake_minimum_required(VERSION 2.8)

set(LIB_NAME libcserialport)

if(APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
endif()


# static libarary
# add_compile_options(-static)

include_directories(../include)

set(COMMON_SOURCES ../src/SerialPort.cpp ../src/SerialPortBase.cpp ../src/SerialPortInfo.cpp ../src/SerialPortInfoBase.cpp)

if (CMAKE_HOST_WIN32)
	set(OS_ABOUT_SOURCES ../src/SerialPortInfoWinBase.cpp ../src/SerialPortWinBase.cpp)
	if (BUILD_SHARED_LIBS)
	    list(APPEND OS_ABOUT_SOURCES version.rc)
	endif()
elseif (CMAKE_HOST_UNIX)
	set(OS_ABOUT_SOURCES ../src/SerialPortInfoUnixBase.cpp ../src/SerialPortUnixBase.cpp)
endif ()

add_library( ${LIB_NAME} ${COMMON_SOURCES} ${OS_ABOUT_SOURCES})

# remove prefix
set_target_properties( ${LIB_NAME} PROPERTIES PREFIX "")

if (CMAKE_HOST_WIN32)
	# for function availableFriendlyPorts
	target_link_libraries( ${LIB_NAME} setupapi )
	#target_link_libraries( ${LIB_NAME} libgcc.a)
	#target_link_libraries( ${LIB_NAME} libstdc++.a)
	#target_link_libraries( ${LIB_NAME} libpthread.a)
elseif(APPLE)
    target_link_libraries( ${LIB_NAME} ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif (CMAKE_HOST_UNIX)
	target_link_libraries( ${LIB_NAME} pthread )
endif()

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/CSerialPort DESTINATION include FILES_MATCHING PATTERN "*.h")

install(TARGETS libcserialport
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		)

install(FILES ${PROJECT_SOURCE_DIR}/cserialport-config.cmake.in DESTINATION lib/cmake/CSerialPort RENAME cserialport-config.cmake)