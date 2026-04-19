#ifndef __C_CSERIALPORT_H__
#define __C_CSERIALPORT_H__

#include <stdint.h> // uintptr_t
typedef uintptr_t i_handle_t;

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
    StopOne = 1,
    StopTwo = 2,
    StopOneAndHalf = 3
};

enum FlowControl
{
    FlowNone = 0,
    FlowHardware = 1,
    FlowSoftware = 2
};

enum SerialPortError
{
    ErrorOK = 0,

    ErrorFail = -1,
    ErrorNotImplemented = -2,
    ErrorInner = -3,
    ErrorNullPointer = -4,
    ErrorInvalidParam = -5,
    ErrorAccessDenied = -6,
    ErrorOutOfMemory = -7,
    ErrorTimeout = -8,

    ErrorNotInit = -20,
    ErrorInitFailed = -21,
    ErrorAlreadyExist = -22,
    ErrorNotExist = -23,
    ErrorAlreadyOpen = -24,
    ErrorNotOpen = -25,
    ErrorOpenFailed = -26,
    ErrorCloseFailed = -27,
    ErrorWriteFailed = -28,
    ErrorReadFailed = -29,

    ErrorUnknown = -999,
};

#ifdef __cplusplus
extern "C"
{
#endif
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

    C_DLL_EXPORT int CSerialPortReadData(i_handle_t handle, void *data, int size);

    C_DLL_EXPORT int CSerialPortReadAllData(i_handle_t handle, void *data);

    C_DLL_EXPORT int CSerialPortWriteData(i_handle_t handle, const void *data, int size);

#if !defined(CSERIALPORT_NATIVE_SYNC)
    typedef void (*pFunReadEvent)(i_handle_t /*handle*/, const char * /*portName*/, unsigned int /*readBufferLen*/);

    typedef void (*pFunHotPlugEvent)(i_handle_t /*handle*/, const char * /*portName*/, int /*isAdd*/);

    typedef unsigned int (*pFunProtocolParser)(i_handle_t /*handle*/, const void * /*buffer*/, unsigned int /*size*/, struct ProtocolResultArray * /*results*/);

    typedef void (*pFunProtocolEvent)(i_handle_t /*handle*/, struct ProtocolResult * /*result*/);
	
    C_DLL_EXPORT int CSerialPortConnectReadEvent(i_handle_t handle, pFunReadEvent pFun);

    C_DLL_EXPORT int CSerialPortDisconnectReadEvent(i_handle_t handle);

    C_DLL_EXPORT int CSerialPortConnectHotPlugEvent(i_handle_t handle, pFunHotPlugEvent pFun);

    C_DLL_EXPORT int CSerialPortDisconnectHotPlugEvent(i_handle_t handle);

    C_DLL_EXPORT int CSerialPortSetProtocolParser(i_handle_t handle, pFunProtocolParser pParser, pFunProtocolEvent pFun);

    C_DLL_EXPORT void CSerialPortSetReadIntervalTimeout(i_handle_t handle, unsigned int msecs);

    C_DLL_EXPORT unsigned int CSerialPortGetReadIntervalTimeout(i_handle_t handle);

    C_DLL_EXPORT void CSerialPortSetMinByteReadNotify(i_handle_t handle, unsigned int minByteReadNotify);

    C_DLL_EXPORT void CSerialPortSetByteReadBufferFullNotify(i_handle_t handle, unsigned int byteReadBufferFullNotify);
#endif

    C_DLL_EXPORT unsigned int CSerialPortGetReadBufferUsedLen(i_handle_t handle);

    C_DLL_EXPORT void CSerialPortSetDebugModel(i_handle_t handle, int isDebug);

    C_DLL_EXPORT int CSerialPortFlushBuffers(i_handle_t handle);

    C_DLL_EXPORT int CSerialPortFlushReadBuffers(i_handle_t handle);

    C_DLL_EXPORT int CSerialPortFlushWriteBuffers(i_handle_t handle);

    C_DLL_EXPORT int CSerialPortGetLastError(i_handle_t handle);

    C_DLL_EXPORT const char *CSerialPortGetLastErrorMsg(i_handle_t handle);

    C_DLL_EXPORT const char *CSerialPortGetErrorMsg(i_handle_t handle, int code);

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