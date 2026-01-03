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

cmake_minimum_required(VERSION 3.10.0)

project(libcserialport)

# C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CSERIALPORT_ENABLE_DEBUG)
    add_definitions(-DCSERIALPORT_DEBUG) # CSerialPort Debug Mode
endif ()
if (CSERIALPORT_ENABLE_UTF8)
    add_definitions(-DCSERIALPORT_USE_UTF8) # CSerialPort UTF8 Character Encoding
endif ()

# static library
# add_compile_options(-static)

if (NOT MSVC)
# unix clang-tidy
# $ run-clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# include source files if CSerialPortSourceFiles is empty
if(NOT CSerialPortSourceFiles)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/cserialport_src.cmake)
endif()

add_library( ${PROJECT_NAME} ${CSerialPortHeaderFiles} ${CSerialPortSourceFiles})

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

cserialport_target_link_libraries(${PROJECT_NAME})

install(DIRECTORY ../include/CSerialPort DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN "*Base.h" EXCLUDE)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(FILES ../cserialport-config.cmake.in DESTINATION lib/cmake/CSerialPort RENAME cserialport-config.cmake)