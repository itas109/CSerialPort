/**
 * @file osplatformutil.h
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * QQ Group : 12951803
 * @brief os platform util 操作系统平台工具
 * @copyright The CSerialPort is Copyright (C) 2021 itas109. \n
 * Contact: itas109@qq.com \n\n
 *  You may use, distribute and copy the CSerialPort under the terms of \n
 *  GNU Lesser General Public License version 3, which is displayed below.
 */
#ifndef OSPLATFORMUTIL_H
#define OSPLATFORMUTIL_H

/*
   The operating system, must be one of: (I_OS_x)

     WIN      - Win (Windows 2000/XP/Vista/7 and Windows Server 2003/2008)
     LINUX    - Linux
     MAC      - macOS, iOS, watchOS, tvOS
     ANDROID  - Android
     FREEBSD  - FreeBSD
     OPENBSD  - OpenBSD
     SOLARIS  - Sun Solaris
     AIX      - AIX
     UNIX     - Any UNIX BSD/SYSV system
*/

#define OS_PLATFORM_UTIL_VERSION 1.0.1.200418

// os platform
// Windows
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(WIN64) || defined(_WIN64) || \
     defined(__WIN64__) || defined(__NT__))
#define I_OS_WIN
// LINUX
#elif defined(__linux__) || defined(__linux)
#define I_OS_LINUX
// MAC
#elif defined(__APPLE__)
#define I_OS_MAC
// ANDROID
#elif defined(__ANDROID__) || defined(ANDROID)
#define I_OS_ANDROID
#define I_OS_LINUX
// sun os
#elif defined(__sun) || defined(sun)
#define I_OS_SOLARIS
// FREEBSD
#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(__FreeBSD_kernel__)
#ifndef __FreeBSD_kernel__
#define I_OS_FREEBSD
#endif
#define I_OS_FREEBSD_KERNEL
// OPENBSD
#elif defined(__OpenBSD__)
#define I_OS_OPENBSD
// IBM AIX
#elif defined(_AIX)
#define I_OS_AIX
#else
#error "not support this OS"
#endif

#if defined(I_OS_WIN)
#undef I_OS_UNIX
#elif !defined(I_OS_UNIX)
#define I_OS_UNIX
#endif

#endif // OSPLATFORMUTIL_H