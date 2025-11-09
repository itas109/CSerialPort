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
#include <string.h> // strchr

// macro to string
#define STRINGIFY(x) #x
#define MACRO_TO_STRING(x) STRINGIFY(x)

// get cpu cores headers
#if defined(__APPLE__)
#include <sys/sysctl.h> // sysctlbyname
#elif defined(__ANDROID__)
#include <unistd.h>                // sysconf
#include <sys/system_properties.h> // __system_property_get
#elif defined(_WIN32)
#include <windows.h> // GetSystemInfo
#include <tchar.h>   // _T

#if defined(_MSC_VER)
#pragma comment(lib, "advapi32.lib") // RegQueryValueEx RegCloseKey
#endif

#elif defined(__linux__)
#include <unistd.h> // sysconf
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__)
#include <sys/sysctl.h> // sysctlbyname

#elif defined(__unix__)
#include <unistd.h> // sysconf
#else
#endif

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

    static size_t strlen(const char *str) noexcept
    {
        // assert(str != NULL);

        const char *cp = str;

        while (*cp++)
        {
        }

        return (cp - str - 1);
    }

    static char *strncpy(char *dest, const char *src, unsigned int count) noexcept
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

    static char *strncat(char *dest, const char *src, unsigned int count) noexcept
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

    static int strFind(const char *src, const char *str) noexcept
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

    static char *strUpper(char *str) noexcept
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

    static char *strLower(char *str) noexcept
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

#if defined(_WIN32)
    static char *WCharToNativeMB(char *dest, unsigned int len, const wchar_t *wstr)
    {
        // wide char to native multi byte(ANSI)
        if (NULL == wstr || NULL == dest || 0 == len)
        {
            return NULL;
        }

        int clen = ::WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
        if (clen <= 0 || (unsigned int)clen > len)
        {
            return NULL;
        }
        // char dest = new char[clen]; // length conatin '\0'
        // ::memset((void *)dest, '\0', clen); // sizeof(char)=1
        if (0 == ::WideCharToMultiByte(CP_ACP, 0, wstr, -1, dest, clen, NULL, NULL))
        {
            return NULL;
        }
        // dest[clen-1] = '\0'; // redundant but safe

        return dest;
    }

    static char *WCharToUTF8(char *dest, unsigned int len, const wchar_t *wstr)
    {
        // wide char to UTF8
        if (NULL == wstr || NULL == dest || 0 == len)
        {
            return NULL;
        }

        int clen = ::WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
        if (clen <= 0 || (unsigned int)clen > len)
        {
            return NULL;
        }
        // char dest = new char[clen]; // length conatin '\0'
        // ::memset((void *)dest, '\0', clen); // sizeof(char)=1
        if (0 == ::WideCharToMultiByte(CP_UTF8, 0, wstr, -1, dest, clen, NULL, NULL))
        {
            return NULL;
        }
        // dest[clen-1] = '\0'; // redundant but safe

        return dest;
    }

    static wchar_t *NativeMBToWChar(wchar_t *dest, unsigned int len, const char *str)
    {
        // native multi byte(ANSI) to wide char
        if (NULL == str || NULL == dest || 0 == len)
        {
            return NULL;
        }

        int wlen = ::MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
        if (wlen <= 0 || (unsigned int)wlen > len)
        {
            return NULL;
        }
        // wchar_t*  dest = new wchar_t[wlen]; // length conatin '\0'
        // ::memset((void *)dest, '\0', wlen * sizeof(wchar_t)); // sizeof(wchar_t)=2
        if (0 == ::MultiByteToWideChar(CP_ACP, 0, str, -1, dest, wlen))
        {
            return NULL;
        }
        // dest[wlen-1] = '\0'; // redundant but safe

        return dest;
    }

    static wchar_t *UTF8ToWChar(wchar_t *dest, unsigned int len, const char *str)
    {
        // UTF8 to wide char
        if (NULL == str || NULL == dest || 0 == len)
        {
            return NULL;
        }

        int wlen = ::MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
        if (wlen <= 0 || (unsigned int)wlen > len)
        {
            return NULL;
        }
        // wchar_t*  dest = new wchar_t[wlen]; // length conatin '\0'
        // ::memset((void *)dest, '\0', wlen * sizeof(wchar_t)); // sizeof(wchar_t)=2
        if (0 == ::MultiByteToWideChar(CP_UTF8, 0, str, -1, dest, wlen))
        {
            return NULL;
        }
        // dest[wlen-1] = '\0'; // redundant but safe

        return dest;
    }
#endif

    static char *charToHexStr(char *dest, const char *src, unsigned int count) noexcept
    {
        // assert(dest != NULL && src != NULL);

        static constexpr char hexTable[17] = "0123456789ABCDEF";

        for (unsigned int i = 0; i < count; ++i)
        {
            dest[i * 2] = hexTable[(unsigned char)src[i] / 16];
            dest[i * 2 + 1] = hexTable[(unsigned char)src[i] % 16];
        }
        dest[count * 2] = '\0';

        return (dest);
    }

    static char *getOperatingSystemName(char *osName, unsigned int len) noexcept
    {
        if (NULL == osName)
        {
            return NULL;
        }

#if defined(__APPLE__)
        strncpy(osName, "macOS", len);
#elif defined(__ANDROID__)
        strncpy(osName, "Android", len);
#elif defined(_WIN32)
        strncpy(osName, "Windows", len);
#elif defined(__linux__)
        strncpy(osName, "Linux", len);
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__)
        strncpy(osName, "BSD", len);
#elif defined(__unix__)
        strncpy(osName, "Unix", len);
#else
        strncpy(osName, "Unknown", len);
#endif
        return osName;
    }

    static char *getProductName(char *productName, unsigned int len)
    {
        if (NULL == productName)
        {
            return NULL;
        }

#if defined(__APPLE__)
        strncpy(productName, "macOS", len);
#elif defined(__ANDROID__)
        char version[15];
        __system_property_get("ro.build.version.release", version);
        strncpy(productName, "Android ", len);
        strncat(productName, version, 15);
#elif defined(_WIN32)
        int majorVersion = 0;
        int minorVersion = 0;
        HKEY hKey;
        if (0 == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), 0, KEY_READ, &hKey))
        {
            DWORD size = 10;
            TCHAR currentVersion[10];
            currentVersion[0] = '\0';

            RegQueryValueEx(hKey, _T("CurrentVersion"), NULL, NULL, (BYTE *)currentVersion, &size);

#ifdef UNICODE
            char currentVersionChar[10];
            WCharToNativeMB(currentVersionChar, 10, currentVersion);
            strScan(currentVersionChar, "%d.%d", &majorVersion, &minorVersion);
#else
            strScan(currentVersion, "%d.%d", &majorVersion, &minorVersion);
#endif

            RegCloseKey(hKey);
            hKey = NULL;
        }

        // IsWindowsServer()
        OSVERSIONINFOEX osvi = {sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0, 0, VER_NT_WORKSTATION};
        DWORDLONG const dwlConditionMask = VerSetConditionMask(0, VER_PRODUCT_TYPE, VER_EQUAL);
        bool isWindowsClient = VerifyVersionInfo(&osvi, VER_PRODUCT_TYPE, dwlConditionMask);

        // https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-osversioninfoexa
        if (10 == majorVersion && 0 == minorVersion)
        {
            strncpy(productName, isWindowsClient ? "win10Preview(10.0)" : "winServer2016(10.0)", len);
        }
        else if (6 == majorVersion && 3 == minorVersion)
        {
            strncpy(productName, isWindowsClient ? "win8.1/10/11(6.3)" : "winServer2012R2(6.3)", len);
        }
        else if (6 == majorVersion && 2 == minorVersion)
        {
            strncpy(productName, isWindowsClient ? "win8(6.2)" : "winServer2012(6.2)", len);
        }
        else if (6 == majorVersion && 1 == minorVersion)
        {
            strncpy(productName, isWindowsClient ? "win7(6.1)" : "winServer2008R2(6.1)", len);
        }
        else if (6 == majorVersion && 0 == minorVersion)
        {
            strncpy(productName, isWindowsClient ? "winVista(6.0)" : "winServer2008(6.0)", len);
        }
        else if (5 == majorVersion && 2 == minorVersion)
        {
            strncpy(productName, isWindowsClient ? "winXP64Bit(5.2)" : "winServer2003(5.2)", len);
        }
        else if (5 == majorVersion && 1 == minorVersion)
        {
            strncpy(productName, "winXP(5.1)", len);
        }
        else
        {
            strFormat(productName, len, "winUnknown(%d.%d)", majorVersion, minorVersion);
        }

#elif defined(__linux__)
        char prettyName[256] = "Linux";
        FILE *fp = fopen("/etc/os-release", "r");
        if (NULL != fp)
        {
            char line[256];

            while (fgets(line, sizeof(line), fp))
            {
                if (0 == strFind(line, "PRETTY_NAME="))
                {
                    // PRETTY_NAME="Ubuntu 22.04.2 LTS" PRETTY_NAME="CentOS Linux 7 (Core)"
                    // strScan(line, "PRETTY_NAME=\"%[^\"]\"", &prettyName); // orangerv2 Noble g++13 crash
                    char *start = line + 13;
                    char *end = NULL;
                    end = strchr(start, '"');
                    int count = end - start + 1;
                    if (end && count < len)
                    {
                        strncpy(prettyName, start, count);
                    }
                    break;
                }
            }

            fclose(fp);
        }

        strncpy(productName, prettyName, len);
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__)
        strncpy(productName, "BSD", len);
#elif defined(__unix__)
        strncpy(productName, "Unix", len);
#else
        strncpy(productName, "Unknown", len);
#endif
        return productName;
    }

    static int getNumberOfProcessors()
    {
        int numOfProcessors = 0;

#if defined(__APPLE__)
        size_t size = sizeof(numOfProcessors);
        sysctlbyname("hw.ncpu", &numOfProcessors, &size, NULL, 0);
#elif defined(__ANDROID__)
        numOfProcessors = sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(_WIN32)
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        numOfProcessors = sysInfo.dwNumberOfProcessors;
#elif defined(__linux__)
        numOfProcessors = sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__)
        size_t size = sizeof(numOfProcessors);
        sysctlbyname("hw.ncpu", &numOfProcessors, &size, NULL, 0);
#elif defined(__unix__)
        numOfProcessors = sysconf(_SC_NPROCESSORS_ONLN);
#else
        // numOfProcessors = 0;
#endif
        return numOfProcessors;
    }

    static char *getArchitectureName(char *archName, unsigned int len) noexcept
    {
        if (NULL == archName)
        {
            return NULL;
        }

        // https://sourceforge.net/p/predef/wiki/Home/
#if defined(__x86_64__) /*GNU C*/ || defined(_M_AMD64) /*Visual Studio*/
        strFormat(archName, len, "x86_64");
#elif defined(__i386__) /*GNU C*/ || defined(_M_IX86)     /*Visual Studio*/
        strFormat(archName, len, "x86");
#elif defined(__arm__) /*GNU C*/ || defined(_M_ARM)       /*Visual Studio*/
        strFormat(archName, len, "arm32");
#elif defined(__aarch64__) /*GNU C*/ || defined(_M_ARM64) /*Visual Studio*/
        strFormat(archName, len, "aarch64");
#elif defined(__mips__)                                   /*GNU C*/
        strFormat(archName, len, "mips");
#elif defined(__riscv)                                    /*GNU C*/
        strFormat(archName, len, "riscv");
#elif defined(__powerpc__)                                /*GNU C*/
        strFormat(archName, len, "powerpc");
#else
        strFormat(archName, len, "unknown");
#endif
        return archName;
    }

    static char *getCompilerName(char *compilerName, unsigned int len) noexcept
    {
        if (NULL == compilerName)
        {
            return NULL;
        }
#if defined(__clang__)
        strFormat(compilerName, len, "clang(%d.%d.%d)", __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(__GNUC__)
        strFormat(compilerName, len, "gcc(%d.%d.%d)", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
        switch (_MSC_VER / 10)
        {
            case 120:
                strFormat(compilerName, len, "vs6.0(%d)", _MSC_VER);
                break;
            case 130:
                strFormat(compilerName, len, "vs2002(%d)", _MSC_VER);
                break;
            case 131:
                strFormat(compilerName, len, "vs2003(%d)", _MSC_VER);
                break;
            case 140:
                strFormat(compilerName, len, "vs2005(%d)", _MSC_VER);
                break;
            case 150:
                strFormat(compilerName, len, "vs2008(%d)", _MSC_VER);
                break;
            case 160:
                strFormat(compilerName, len, "vs2010(%d)", _MSC_VER);
                break;
            case 170:
                strFormat(compilerName, len, "vs2012(%d)", _MSC_VER);
                break;
            case 180:
                strFormat(compilerName, len, "vs2013(%d)", _MSC_VER);
                break;
            case 190:
                strFormat(compilerName, len, "vs2015(%d)", _MSC_VER);
                break;
            case 191:
                strFormat(compilerName, len, "vs2017(%d)", _MSC_VER);
                break;
            case 192:
                strFormat(compilerName, len, "vs2019(%d)", _MSC_VER);
                break;
            case 193:
            case 194:
                strFormat(compilerName, len, "vs2022(%d)", _MSC_VER);
                break;
            default:
                strFormat(compilerName, len, "msvc(unknown)(%d)", _MSC_VER);
                break;
        }
#else
        strFormat(compilerName, len, "unknown(%d.%d.%d)", 0, 0, 0);
#endif
        return compilerName;
    }

    static int getApplicationBit() noexcept
    {
        static int bit = (8 == sizeof(char *)) ? 64 : 32;
        return bit;
    }

    static long getCppStdVersion() noexcept
    {
#if defined(_MSVC_LANG)
        return _MSVC_LANG;
#else
        return __cplusplus;
#endif
    }

    static char *getBindingLanguage(char *bindingLanguage, unsigned int len) noexcept
    {
#ifdef CSERIALPORT_BINDING_LANGUAGE
        strncpy(bindingLanguage, MACRO_TO_STRING(CSERIALPORT_BINDING_LANGUAGE), len);
#endif
        return bindingLanguage;
    }

    static const char *getCompilerInfo(char *info, unsigned int len)
    {
        char osName[10];
        osName[0] = '\0';
        char productName[50];
        productName[0] = '\0';
        char archName[10];
        archName[0] = '\0';
        char compilerName[20];
        compilerName[0] = '\0';
        char compilerVersion[10];
        compilerVersion[0] = '\0';
        char bindingLang[20];
        bindingLang[0] = '\0';

        strFormat(info, len, "OS: %s, ProductName: %s, Arch: %s, ProcessorNum: %d, Compiler: %s, Bit: %d, C++: %ldL, Bindings: %s",
                  getOperatingSystemName(osName, sizeof(osName)),      // OperatingSystemName
                  getProductName(productName, sizeof(productName)),    // ProductName
                  getArchitectureName(archName, sizeof(archName)),     // ArchitectureName
                  getNumberOfProcessors(),                             // NumberOfProcessors
                  getCompilerName(compilerName, sizeof(compilerName)), // CompilerName
                  getApplicationBit(),                                 // application bit 32 or 64
                  getCppStdVersion(),                                  // c++ std version
                  getBindingLanguage(bindingLang, sizeof(bindingLang)) // binding language
        );

        return info;
    }
};
} // namespace itas109
#endif // __I_UTILS_HPP__
