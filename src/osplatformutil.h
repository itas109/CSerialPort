#ifndef OSPLATFORMUTIL_H
#define OSPLATFORMUTIL_H

/*
   The operating system, must be one of: (I_OS_x)

     DARWIN   - Any Darwin system (macOS, iOS, watchOS, tvOS)
	 ANDROID  - Android platform
	 WIN32    - Win32 (Windows 2000/XP/Vista/7 and Windows Server 2003/2008)
	 WINRT    - WinRT (Windows Runtime)
	 CYGWIN   - Cygwin
	 LINUX    - Linux
	 FREEBSD  - FreeBSD
	 OPENBSD  - OpenBSD
	 SOLARIS  - Sun Solaris
	 AIX      - AIX
     UNIX     - Any UNIX BSD/SYSV system
*/

#define OS_PLATFORM_UTIL_VERSION 1.0.0.180723

// DARWIN
#if defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#  include <TargetConditionals.h>
#  if defined(TARGET_OS_MAC) && TARGET_OS_MAC
#    define I_OS_DARWIN
#    ifdef __LP64__
#      define I_OS_DARWIN64
#    else
#      define I_OS_DARWIN32
#    endif
#  else
#    error "not support this Apple platform"
#  endif
// ANDROID
#elif defined(__ANDROID__) || defined(ANDROID)
#  define I_OS_ANDROID
#  define I_OS_LINUX
// Windows
#elif !defined(SAG_COM) && (!defined(WINAPI_FAMILY) || WINAPI_FAMILY==WINAPI_FAMILY_DESKTOP_APP) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define I_OS_WIN32
#  define I_OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  if defined(WINAPI_FAMILY)
#    ifndef WINAPI_FAMILY_PC_APP
#      define WINAPI_FAMILY_PC_APP WINAPI_FAMILY_APP
#    endif
#    if defined(WINAPI_FAMILY_PHONE_APP) && WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP
#      define I_OS_WINRT
#    elif WINAPI_FAMILY==WINAPI_FAMILY_PC_APP
#      define I_OS_WINRT
#    else
#      define I_OS_WIN32
#    endif
#  else
#    define I_OS_WIN32
#  endif
//CYGWIN
#elif defined(__CYGWIN__)
#  define I_OS_CYGWIN
// sun os
#elif defined(__sun) || defined(sun)
#  define I_OS_SOLARIS
// LINUX
#elif defined(__linux__) || defined(__linux)
#  define I_OS_LINUX
// FREEBSD
#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(__FreeBSD_kernel__)
#  ifndef __FreeBSD_kernel__
#    define I_OS_FREEBSD
#  endif
#  define I_OS_FREEBSD_KERNEL
// OPENBSD
#elif defined(__OpenBSD__)
#  define I_OS_OPENBSD
// IBM AIX
#elif defined(_AIX)
#  define I_OS_AIX
#else
#  error "not support this OS"
#endif

#if defined(I_OS_WIN32) || defined(I_OS_WIN64) || defined(I_OS_WINRT)
#  define I_OS_WIN
#endif

#if defined(I_OS_WIN)
#  undef I_OS_UNIX
#elif !defined(I_OS_UNIX)
#  define I_OS_UNIX
#endif

#ifdef I_OS_DARWIN
#define I_OS_MAC
#endif
#ifdef I_OS_DARWIN32
#define I_OS_MAC32
#endif
#ifdef I_OS_DARWIN64
#define I_OS_MAC64
#endif

#endif // OSPLATFORMUTIL_H