#ifndef __C_CSERIALPORT_H__
#define __C_CSERIALPORT_H__

#if defined(_MSC_VER) // msvc
#ifdef _WIN64
typedef unsigned __int64 i_handle_t; // windows 64bit
#else
typedef unsigned int i_handle_t; // windows 32bit
#endif
#elif defined(__MINGW64__) // MinGW-w64 (64-bit)
typedef unsigned long long i_handle_t; // windows 64bit (MinGW-w64)
#elif defined(__MINGW32__) // MinGW (32-bit)
typedef unsigned int i_handle_t; // windows 32bit (MinGW)
#elif defined(__LP64__) || defined(_LP64) // gcc/clang
typedef unsigned long i_handle_t; // LP64(long pointer 64) // 64bit OS(unix/linux/macos)
#else
typedef unsigned int i_handle_t; // ILP32(int long pointer 32) // 32bit OS(windows/unix/linux/macos)
#endif

#ifdef _WIN32
#define C_DLL_EXPORT __declspec(dllexport)
#else
#define C_DLL_EXPORT
#endif

struct SerialPortInfo
{
    char portName[256];
    char description[256];
    char hardwareId[256];
};

struct SerialPortInfoArray
{
    struct SerialPortInfo *portInfo;
    unsigned int size;
};

struct ProtocolResult
{
    unsigned char data[256];
    unsigned int len;
};

struct ProtocolResultArray
{
    struct ProtocolResult *result;
    unsigned int size;
};

enum OperateMode
{
    AsynchronousOperate,
    SynchronousOperate
};

enum DataBits
{
    DataBits5 = 5,
    DataBits6 = 6,
    DataBits7 = 7,
    DataBits8 = 8
};

enum Parity
{
    ParityNone = 0,
    ParityOdd = 1,
    ParityEven = 2,
    ParityMark = 3,
    ParitySpace = 4,
};

enum StopBits
{
    StopOne = 0,
    StopOneAndHalf = 1,
    StopTwo = 2
};

enum FlowControl
{
    FlowNone = 0,
    FlowHardware = 1,
    FlowSoftware = 2
};

#ifdef __cplusplus
extern "C"
{
#endif
    typedef void (*pFunReadEvent)(i_handle_t /*handle*/, const char * /*portName*/, unsigned int /*readBufferLen*/);

    typedef void (*pFunHotPlugEvent)(i_handle_t /*handle*/, const char * /*portName*/, int /*isAdd*/);

    typedef unsigned int (*pFunProtocolParser)(i_handle_t /*handle*/, const void * /*buffer*/, unsigned int /*size*/, struct ProtocolResultArray * /*results*/);

    typedef void (*pFunProtocolEvent)(i_handle_t /*handle*/, struct ProtocolResult * /*result*/);

    C_DLL_EXPORT void CSerialPortAvailablePortInfosMalloc(struct SerialPortInfoArray *portInfoArray);

    C_DLL_EXPORT void CSerialPortAvailablePortInfosFree(struct SerialPortInfoArray *portInfoArray);

    C_DLL_EXPORT i_handle_t CSerialPortMalloc();

    C_DLL_EXPORT void CSerialPortFree(i_handle_t handle);

    C_DLL_EXPORT void CSerialPortInit(i_handle_t handle,
                                      const char *portName,
                                      int baudRate,
                                      enum Parity parity,
                                      enum DataBits dataBits,
                                      enum StopBits stopbits,
                                      enum FlowControl flowControl,
                                      unsigned int readBufferSize);

    C_DLL_EXPORT void CSerialPortSetOperateMode(i_handle_t handle, enum OperateMode operateMode);

    C_DLL_EXPORT int CSerialPortOpen(i_handle_t handle);

    C_DLL_EXPORT void CSerialPortClose(i_handle_t handle);

    C_DLL_EXPORT int CSerialPortIsOpen(i_handle_t handle);

    C_DLL_EXPORT int CSerialPortConnectReadEvent(i_handle_t handle, pFunReadEvent pFun);

    C_DLL_EXPORT int CSerialPortDisconnectReadEvent(i_handle_t handle);

    C_DLL_EXPORT int CSerialPortConnectHotPlugEvent(i_handle_t handle, pFunHotPlugEvent pFun);

    C_DLL_EXPORT int CSerialPortDisconnectHotPlugEvent(i_handle_t handle);

    C_DLL_EXPORT int CSerialPortSetProtocolParser(i_handle_t handle, pFunProtocolParser pParser,pFunProtocolEvent pFun);

    C_DLL_EXPORT unsigned int CSerialPortGetReadBufferUsedLen(i_handle_t handle);

    C_DLL_EXPORT int CSerialPortReadData(i_handle_t handle, void *data, int size);

    C_DLL_EXPORT int CSerialPortReadAllData(i_handle_t handle, void *data);

    C_DLL_EXPORT int CSerialPortWriteData(i_handle_t handle, const void *data, int size);

    C_DLL_EXPORT void CSerialPortSetDebugModel(i_handle_t handle, int isDebug);

    C_DLL_EXPORT void CSerialPortSetReadIntervalTimeout(i_handle_t handle, unsigned int msecs);

    C_DLL_EXPORT unsigned int CSerialPortGetReadIntervalTimeout(i_handle_t handle);

    C_DLL_EXPORT void CSerialPortSetMinByteReadNotify(i_handle_t handle, unsigned int minByteReadNotify);

    C_DLL_EXPORT int CSerialPortFlushBuffers(i_handle_t handle);

    C_DLL_EXPORT int CSerialPortFlushReadBuffers(i_handle_t handle);

    C_DLL_EXPORT int CSerialPortFlushWriteBuffers(i_handle_t handle);

    C_DLL_EXPORT int CSerialPortGetLastError(i_handle_t handle);

    C_DLL_EXPORT const char *CSerialPortGetLastErrorMsg(i_handle_t handle);

    C_DLL_EXPORT void CSerialPortClearError(i_handle_t handle);

    C_DLL_EXPORT void CSerialPortSetPortName(i_handle_t handle, const char *portName);

    C_DLL_EXPORT const char *CSerialPortGetPortName(i_handle_t handle);

    C_DLL_EXPORT void CSerialPortSetBaudRate(i_handle_t handle, int baudRate);

    C_DLL_EXPORT int CSerialPortGetBaudRate(i_handle_t handle);

    C_DLL_EXPORT void CSerialPortSetParity(i_handle_t handle, enum Parity parity);

    C_DLL_EXPORT enum Parity CSerialPortGetParity(i_handle_t handle);

    C_DLL_EXPORT void CSerialPortSetDataBits(i_handle_t handle, enum DataBits dataBits);

    C_DLL_EXPORT enum DataBits CSerialPortGetDataBits(i_handle_t handle);

    C_DLL_EXPORT void CSerialPortSetStopBits(i_handle_t handle, enum StopBits stopbits);

    C_DLL_EXPORT enum StopBits CSerialPortGetStopBits(i_handle_t handle);

    C_DLL_EXPORT void CSerialPortSetFlowControl(i_handle_t handle, enum FlowControl flowControl);

    C_DLL_EXPORT enum FlowControl CSerialPortGetFlowControl(i_handle_t handle);

    C_DLL_EXPORT void CSerialPortSetReadBufferSize(i_handle_t handle, unsigned int size);

    C_DLL_EXPORT unsigned int CSerialPortGetReadBufferSize(i_handle_t handle);

    C_DLL_EXPORT void CSerialPortSetDtr(i_handle_t handle, int set);

    C_DLL_EXPORT void CSerialPortSetRts(i_handle_t handle, int set);

    C_DLL_EXPORT const char *CSerialPortGetVersion(i_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif // !__C_CSERIALPORT_H__