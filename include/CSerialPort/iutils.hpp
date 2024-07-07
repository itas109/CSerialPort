/**
 * @file iutils.hpp
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief lightweight cross-platform utils library 轻量级跨平台工具类
 */
#ifndef __I_UTILS_HPP__
#define __I_UTILS_HPP__

#include <stdarg.h> // va_start
#include <stdio.h>  // vsprintf

namespace itas109
{
/**
 * @brief utils class 工具类
 *
 * @tparam T
 */
class IUtils
{
public:
    IUtils() {};
    ~IUtils() {};

    static size_t strlen(const char *str)
    {
        // assert(str != NULL);

        const char *cp = str;

        while (*cp++)
        {
        }

        return (cp - str - 1);
    }

    static char *strncpy(char *dest, const char *src, unsigned int count)
    {
        // assert(dest != NULL && src != NULL && count != 0);

        char *cp = dest;

        while (--count && (*cp++ = *src++) != '\0')
        {
        }

        if (0 == count)
        {
            *cp = '\0';
        }
        return (dest);
    }

    static char *strncat(char *dest, const char *src, unsigned int count)
    {
        // assert(dest != NULL && src != NULL);

        char *cp = dest;

        while (*cp) // find dest end
        {
            cp++;
        }

        while (--count && (*cp++ = *src++) != '\0')
        {
        }

        if (0 == count)
        {
            *cp = '\0';
        }
        return (dest);
    }

    static int strFind(const char *src, const char *str)
    {
        // assert(dest != NULL && src != NULL);

        char *cp = (char *)src;
        char *s1, *s2;

        if (!*str)
        {
            return 0;
        }
        while (*cp)
        {
            s1 = cp;
            s2 = (char *)str;
            while (*s2 && !(*s1 - *s2))
            {
                s1++;
                s2++;
            }
            if (!*s2)
            {
                return (int)(cp - src);
            }
            cp++;
        }
        return -1;
    }

    static char *strUpper(char *str)
    {
        char *cp = str;
        for (; *str != '\0'; str++)
        {
            if ((*str >= 'a') && (*str <= 'z'))
            {
                *str = *str + ('A' - 'a');
            }
        }
        return cp;
    }

    static char *strLower(char *str)
    {
        char *cp = str;
        for (; *str != '\0'; str++)
        {
            if ((*str >= 'A') && (*str <= 'Z'))
            {
                *str = *str + ('a' - 'A');
            }
        }
        return cp;
    }

    static int strFormat(char *str, size_t len, const char *format, va_list args)
    {
        int ret;
#ifdef _MSC_VER
        ret = vsnprintf_s(str, len, _TRUNCATE, format, args);
#else
        ret = vsnprintf(str, len, format, args);
#endif
        return ret;
    }

    static int strFormat(char *str, size_t len, const char *format, ...)
    {
        va_list ap;
        int ret;

        va_start(ap, format);
        ret = itas109::IUtils::strFormat(str, len, format, ap);
        va_end(ap);

        return ret;
    }

    static int strScan(const char *str, const char *format, va_list args)
    {
        int ret;

#if defined(_MSC_VER)
#if _MSC_VER < 1800 // vs2013
        void *a[8];
        for (int i = 0; i < sizeof(a) / sizeof(a[0]); ++i)
        {
            a[i] = va_arg(args, void *);
        }
        ret = sscanf_s(str, format, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8]);
#else
        ret = vsscanf_s(str, format, args);
#endif
#else
        ret = vsscanf(str, format, args);
#endif

        return ret;
    }

    static int strScan(const char *str, const char *format, ...)
    {
        va_list ap;
        int ret;

        va_start(ap, format);
        ret = itas109::IUtils::strScan(str, format, ap);
        va_end(ap);

        return ret;
    }

    static char *charToHexStr(char *dest, const char *src, unsigned int count)
    {
        // assert(dest != NULL && src != NULL);

        static const char hexTable[17] = "0123456789ABCDEF";

        for (unsigned int i = 0; i < count; ++i)
        {
            dest[i * 2] = hexTable[(unsigned char)src[i] / 16];
            dest[i * 2 + 1] = hexTable[(unsigned char)src[i] % 16];
        }
        dest[count * 2] = '\0';

        return (dest);
    }

    static const char *getCompilerInfo(char *info, size_t len)
    {
        char osName[10];
        osName[0] = '\0';
        char archName[10];
        archName[0] = '\0';
        char compilerName[10];
        compilerName[0] = '\0';
        char compilerVersion[10];
        compilerVersion[0] = '\0';
        int bit = (8 == sizeof(char *)) ? 64 : 32;
        long cppStdVersion = 0;

        // https://sourceforge.net/p/predef/wiki/Home/
#if defined(__x86_64__) /*GNU C*/ || defined(_M_AMD64) /*Visual Studio*/
        strFormat(archName, 10, "x86_64");
#elif defined(__i386__) /*GNU C*/ || defined(_M_IX86)     /*Visual Studio*/
        strFormat(archName, 10, "x86");
#elif defined(__arm__) /*GNU C*/ || defined(_M_ARM)       /*Visual Studio*/
        strFormat(archName, 10, "arm32");
#elif defined(__aarch64__) /*GNU C*/ || defined(_M_ARM64) /*Visual Studio*/
        strFormat(archName, 10, "aarch64");
#elif defined(__mips__)                                   /*GNU C*/
        strFormat(archName, 10, "mips");
#elif defined(__riscv)                                    /*GNU C*/
        strFormat(archName, 10, "riscv");
#elif defined(__powerpc__)                                /*GNU C*/
        strFormat(archName, 10, "powerpc");
#else
        strFormat(archName, 10, "unknown");
#endif

#if defined(_WIN32)
        strncpy(osName, "Windows", 10);
#elif defined(__linux__)
        strncpy(osName, "Linux", 10);
#elif defined(__APPLE__)
        strncpy(osName, "MacOS", 10);
#elif defined(__ANDROID__)
        strncpy(osName, "Android", 10);
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__)
        strncpy(osName, "BSD", 10);
#elif defined(__unix__)
        strncpy(osName, "Unix", 10);
#else
        strncpy(osName, "Unknown", 10);
#endif

#if defined(__clang__)
        strncpy(compilerName, "clang", 10);
        strFormat(compilerVersion, 10, "%d.%d.%d", __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(__GNUC__)
        strncpy(compilerName, "gcc", 10);
        strFormat(compilerVersion, 10, "%d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
        strFormat(compilerVersion, 10, "%d", _MSC_VER);
        switch (_MSC_VER / 10)
        {
            case 120:
                strncpy(compilerName, "vs6.0", 10);
                break;
            case 130:
                strncpy(compilerName, "vs2002", 10);
                break;
            case 131:
                strncpy(compilerName, "vs2003", 10);
                break;
            case 140:
                strncpy(compilerName, "vs2005", 10);
                break;
            case 150:
                strncpy(compilerName, "vs2008", 10);
                break;
            case 160:
                strncpy(compilerName, "vs2010", 10);
                break;
            case 170:
                strncpy(compilerName, "vs2012", 10);
                break;
            case 180:
                strncpy(compilerName, "vs2013", 10);
                break;
            case 190:
                strncpy(compilerName, "vs2015", 10);
                break;
            case 191:
                strncpy(compilerName, "vs2017", 10);
                break;
            case 192:
                strncpy(compilerName, "vs2019", 10);
                break;
            case 193:
                strncpy(compilerName, "vs2022", 10);
                break;
        }
#else
        strncpy(compilerName, "unknown", 10);
        strFormat(compilerVersion, 10, "%d.%d.%d", 0, 0, 0);
#endif

#if defined(_MSVC_LANG)
        cppStdVersion = _MSVC_LANG;
#else
        cppStdVersion = __cplusplus;
#endif

        strFormat(info, len, "OS: %s, Arch: %s, Compiler: %s(%s), Bit: %d, C++: %ldL", osName, archName, compilerName, compilerVersion, bit, cppStdVersion);

        return info;
    }
};
} // namespace itas109
#endif // __I_UTILS_HPP__
