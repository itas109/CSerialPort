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

project(libcserialport)

# static libarary
# add_compile_options(-static)

set(CSerialPortRootPath "${CMAKE_CURRENT_SOURCE_DIR}/..")
include_directories(${CSerialPortRootPath}/include)
list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPort.cpp ${CSerialPortRootPath}/src/SerialPortBase.cpp ${CSerialPortRootPath}/src/SerialPortInfo.cpp ${CSerialPortRootPath}/src/SerialPortInfoBase.cpp)
if (WIN32)
    list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortInfoWinBase.cpp ${CSerialPortRootPath}/src/SerialPortWinBase.cpp)
    if (BUILD_SHARED_LIBS)
        list(APPEND CSerialPortSourceFiles version.rc)
    endif()
elseif (UNIX)
    list(APPEND CSerialPortSourceFiles ${CSerialPortRootPath}/src/SerialPortInfoUnixBase.cpp ${CSerialPortRootPath}/src/SerialPortUnixBase.cpp)
endif ()

if (NOT MSVC)
# unix clang-tidy
# $ run-clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

add_library( ${PROJECT_NAME} ${CSerialPortSourceFiles})

if(MSVC)
# msvc clang-tidy
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_RunCodeAnalysis true
    VS_GLOBAL_EnableMicrosoftCodeAnalysis false
    VS_GLOBAL_CodeAnalysisRuleSet NativeRecommendedRules.ruleset
    VS_GLOBAL_EnableClangTidyCodeAnalysis true
)
endif()

# remove prefix
set_target_properties( ${PROJECT_NAME} PROPERTIES PREFIX "")

# preprocessor definitions for compiling CSerialPort library
set_target_properties( ${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS BUILDING_LIBCSERIALPORT)

if (WIN32)
	# for function availableFriendlyPorts
	target_link_libraries( ${PROJECT_NAME} setupapi)
	#target_link_libraries( ${PROJECT_NAME} libgcc.a)
	#target_link_libraries( ${PROJECT_NAME} libstdc++.a)
	#target_link_libraries( ${PROJECT_NAME} libpthread.a)
elseif(APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
    target_link_libraries( ${PROJECT_NAME} ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif (UNIX)
	target_link_libraries( ${PROJECT_NAME} pthread)
endif()

install(DIRECTORY ../include/CSerialPort DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN "*Base.h" EXCLUDE)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(FILES ../cserialport-config.cmake.in DESTINATION lib/cmake/CSerialPort RENAME cserialport-config.cmake)