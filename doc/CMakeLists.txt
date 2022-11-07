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

project(doc)

find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT_CN ${CMAKE_CURRENT_BINARY_DIR}/DoxyfileCN)
    set(DOXYGEN_OUT_EN ${CMAKE_CURRENT_BINARY_DIR}/DoxyfileEN)

    file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../include/CSerialPort/SerialPort_version.h VERSION_CONTENTS REGEX "#define CSERIALPORT_VERSION")
    string(REGEX MATCH "#define CSERIALPORT_VERSION \"[^\"]*" CSERIALPORT_VERSION ${VERSION_CONTENTS})
    string(REGEX REPLACE "[^\"]+\"" "" CSERIALPORT_VERSION ${CSERIALPORT_VERSION})
    set(INPUT ${CMAKE_CURRENT_SOURCE_DIR}/../include/CSerialPort)
    set(OUTPUT ${CMAKE_BINARY_DIR}/bin)

    set(OUTPUT_LANGUAGE "Chinese")
    set(MDFILE ${CMAKE_CURRENT_SOURCE_DIR}/../README_zh_CN.md)
    set(CHM_FILE "CSerialPort_doc_cn.chm")
    set(CHM_INDEX_ENCODING "GB2312")
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT_CN} @ONLY)
    add_custom_target( doc_cn ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT_CN}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    set(OUTPUT_LANGUAGE "English")
    set(MDFILE ${CMAKE_CURRENT_SOURCE_DIR}/../README.md)
    set(CHM_FILE "CSerialPort_doc_en.chm")
    set(CHM_INDEX_ENCODING "")
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT_EN} @ONLY)
    add_custom_target(doc_en ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT_EN}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )

    # doc_cn first
    add_dependencies(doc_en doc_cn)
else ()
  message(FATAL_ERROR "Doxygen not found")
endif ()