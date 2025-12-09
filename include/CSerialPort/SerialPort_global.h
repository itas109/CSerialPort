/**
 * @file SerialPort_global.h
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief global define of CSerialPort 串口全局定义
 * @copyright The CSerialPort is Copyright (C) 2014 itas109 <itas109@qq.com>. \n
 * You may use, copy, modify, and distribute the CSerialPort, under the terms \n
 * of the LICENSE file.
 */
#ifndef __CSERIALPORT_GLOBAL_H__
#define __CSERIALPORT_GLOBAL_H__

#if defined(__APPLE__) // MAC
#define I_OS_MAC
#elif defined(__ANDROID__) || defined(ANDROID) // ANDROID
#define I_OS_ANDROID
#define I_OS_LINUX
#elif defined(_WIN32) // Windows
#define I_OS_WIN
#elif defined(__linux__) || defined(__linux) // LINUX
#define I_OS_LINUX
#else
// #error "not support this OS"
#endif

#if defined(I_OS_WIN)
#undef I_OS_UNIX
#elif !defined(I_OS_UNIX)
#define I_OS_UNIX
#endif

// enum is not a class or namespace error
// https://stackoverflow.com/questions/5188554/my-enum-is-not-a-class-or-namespace

#ifdef I_OS_WIN
#if defined(BUILDING_LIBCSERIALPORT)
#define DLL_EXPORT __declspec(dllexport) ///< define DLL_EXPORT windows 定义windows导出函数
#else
#define DLL_EXPORT
#endif
#else
// https://gcc.gnu.org/wiki/Visibility gcc default export all gcc默认全部导出
#define DLL_EXPORT
#endif // I_OS_WIN

#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE
#endif
#endif
#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif
#endif

// C++11 support
#if (__cplusplus >= 201103L) // gcc4.8.1 clang3.1
#define CSERIALPORT_CPP11
#elif (defined(_MSC_VER) && (_MSC_VER >= 1800)) // msvc 2013(1800) partial support, msvc 2015(1900) full support
#define CSERIALPORT_CPP11
#else
// not support C++11
#error "this compiler not support c++11. please update to gcc4.8+ clang3.1+ or msvc2015+"
#endif

// #ifdef CSERIALPORT_DEBUG
// #include <stdio.h>
//// #include <string.h> // strrchr
// #ifdef _WIN32
// #ifndef __func__
// #define __func__ __FUNCTION__
// #endif
//// #define LOG_INFO(format, ...) printf("[CSERIALPORT_DEBUG] %s - " format " (%s:%d)\n", __func__, ##__VA_ARGS__, (strrchr("\\" __FILE__, '\\') + 1), __LINE__)
// #define LOG_INFO(format, ...) printf("[CSERIALPORT_DEBUG] %s - " format "\n", __func__, ##__VA_ARGS__)
// #else
//// #define LOG_INFO(format, ...) printf("[CSERIALPORT_DEBUG] %s - " format " (%s:%d)\n", __func__, ##__VA_ARGS__, (strrchr("/" __FILE__, '/') + 1), __LINE__)
// #define LOG_INFO(format, ...) printf("[CSERIALPORT_DEBUG] %s - " format "\n", __func__, ##__VA_ARGS__)
// #endif
// #else
// #define LOG_INFO(format, ...)
// #endif

namespace itas109
{
/**
 * @brief serial port info struct 串口信息结构体
 *
 */
struct SerialPortInfo
{
    char portName[256];    ///< portName 串口名称
    char description[256]; ///< description 串口描述
    char hardwareId[256];  ///< hardwareId 硬件id
};

/**
 * @brief the read and write serial port mode enum 读写串口模式
 *
 */
enum OperateMode
{
    AsynchronousOperate,     ///< asynchronous mode based event driven and thread  基于事件驱动和多线程的异步模式
    SynchronousOperate,      ///< synchronous mode based asynchronous mode 基于多线程和事件驱动的异步模式的同步模式
    NativeSynchronousOperate ///< native synchronous mode baseed os sync api(no thead) 基于操作系统同步API(无多线程)的原生同步模式
};

/**
 * @brief the BaudRate enum 波特率
 * @warning because baudrate is number, so it can be any value(includr follow definitions).
 * 因为波特率为数值类型，所以理论上可以为任意值(包括下列定义)
 */
enum BaudRate
{
    BaudRate110 = 110,       ///< 110
    BaudRate300 = 300,       ///< 300
    BaudRate600 = 600,       ///< 600
    BaudRate1200 = 1200,     ///< 1200
    BaudRate2400 = 2400,     ///< 2400
    BaudRate4800 = 4800,     ///< 4800
    BaudRate9600 = 9600,     ///< 9600 recommend 推荐
    BaudRate14400 = 14400,   ///< 14400
    BaudRate19200 = 19200,   ///< 19200
    BaudRate38400 = 38400,   ///< 38400
    BaudRate56000 = 56000,   ///< 56000
    BaudRate57600 = 57600,   ///< 57600
    BaudRate115200 = 115200, ///< 115200
    BaudRate921600 = 921600  ///< 921600
};

/**
 * @brief the DataBits enum 数据位
 * @warning
 * by QextSerialPort:\n
 * 5 data bits cannot be used with 2 stop bits 5位数据位不能使用2位停止位\n
 * 1.5 stop bits can only be used with 5 data bits 1.5位停止位不能使用5位数据位\n
 * 8 data bits cannot be used with space parity on POSIX systems POSIX系统8位数据位不能使用0校验
 * @warning windows Number of bits/byte, 4-8 windows数据位范围为4 - 8
 *
 */
enum DataBits
{
    DataBits5 = 5, ///< 5 data bits 5位数据位
    DataBits6 = 6, ///< 6 data bits 6位数据位
    DataBits7 = 7, ///< 7 data bits 7位数据位
    DataBits8 = 8  ///< 8 data bits 8位数据位
};

/**
 * @brief the Parity enum 校验位
 * @warning windows 0-4=None,Odd,Even,Mark,Space
 *
 */
enum Parity
{
    ParityNone = 0,  ///< No Parity 无校验
    ParityOdd = 1,   ///< Odd Parity 奇校验
    ParityEven = 2,  ///< Even Parity 偶校验
    ParityMark = 3,  ///< Mark Parity 1校验
    ParitySpace = 4, ///< Space Parity 0校验
};

/**
 * @brief the StopBits enum 停止位
 * @warning 1.5 stop bit only for the Windows platform 1.5位停止位仅对windows有效
 * @warning windows 0,1,2 = 1, 1.5, 2
 *
 */
enum StopBits
{
    StopOne = 0,        ///< 1 stop bit 1位停止位
    StopOneAndHalf = 1, ///< 1.5 stop bit 1.5位停止位 - 1.5 only for windows
    StopTwo = 2         ///< 2 stop bit 2位停止位
};

/**
 * @brief the FlowControl enum 流控制
 *
 */
enum FlowControl
{
    FlowNone = 0,     ///< No flow control 无流控制
    FlowHardware = 1, ///< Hardware(RTS / CTS) flow control 硬件流控制
    FlowSoftware = 2  ///< Software(XON / XOFF) flow control 软件流控制
};

/**
 * @brief the SerialPort error code 串口错误码 0 ok, <0 error
 *
 */
enum SerialPortError
{
    ErrorOK = 0, ///< ok 成功

    ErrorFail = -1,           ///< general error一般性错误
    ErrorNotImplemented = -2, ///< not implemented 未实现
    ErrorInner = -3,          ///< inner error 内部错误(如内存访问异常等)
    ErrorNullPointer = -4,    ///< null pointer error 空指针错误
    ErrorInvalidParam = -5,   ///< invalid parameter error 无效的参数
    ErrorAccessDenied = -6,   ///< access denied error 权限错误
    ErrorOutOfMemory = -7,    ///< out of memory 内存不足
    ErrorTimeout = -8,        ///< time out error 超时

    ErrorNotInit = -20,      ///< not init 未初始化
    ErrorInitFailed = -21,   ///< init failed 初始化失败
    ErrorAlreadyExist = -22, ///< already exist 已经存在
    ErrorNotExist = -23,     ///< not exist 不存在
    ErrorAlreadyOpen = -24,  ///< already open 已经打开
    ErrorNotOpen = -25,      ///< not open 未打开
    ErrorOpenFailed = -26,   ///< open failed 打开失败
    ErrorCloseFailed = -27,  ///< close failed 关闭失败
    ErrorWriteFailed = -28,  ///< write failed 写入失败
    ErrorReadFailed = -29,   ///< read failed 读取失败

    ErrorUnknown = -999, ///< unknown error 未知错误
};
} // namespace itas109

#endif //__CSERIALPORT_GLOBAL_H__
