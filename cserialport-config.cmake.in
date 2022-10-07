#***************************************************************************
# @file cserialport-config.cmake
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

set(CSerialPort_FOUND FALSE)

if(NOT CSerialPort_INCLUDE_DIR)
        find_path(CSerialPort_INCLUDE_DIR NAMES
                CSerialPort/SerialPort.h
                HINTS ${PC_CSerialPort_INCLUDEDIR} ${PC_CSerialPort_INCLUDE_DIRS}
                PATH_SUFFIXES CSerialPort )

        mark_as_advanced(CSerialPort_INCLUDE_DIR)
endif()

if(NOT CSerialPort_LIBRARY)
        find_library(CSerialPort_LIBRARY NAMES
                cserialport 
                libcserialport
                HINTS ${PC_CSerialPort_LIBDIR} ${PC_CSerialPort_LIBRARY_DIRS} 
                )

        mark_as_advanced(CSerialPort_LIBRARY)
endif()

if(CSerialPort_INCLUDE_DIR AND CSerialPort_LIBRARY)
        set(CSerialPort_FOUND TRUE)
        message(STATUS "FOUND CSerialPort, ${CSerialPort_LIBRARY}")
endif()