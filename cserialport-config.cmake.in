# @file cserialport-config.cmake
# @author itas109 (itas109@qq.com) \n\n
# Blog : https://blog.csdn.net/itas109 \n
# Github : https://github.com/itas109 \n
# QQ Group : 12951803
# @brief a lightweight library of serial port, which can easy to read and write serical port on windows and linux with C++ 轻量级跨平台串口读写类库
# @copyright The CSerialPort is Copyright (C) 2021 itas109. \n
# Contact: itas109@qq.com \n\n
#  You may use, distribute and copy the CSerialPort under the terms of \n
#  GNU Lesser General Public License version 3, which is displayed below.

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