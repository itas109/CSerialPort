#ifndef __C_CSERIALPORT_H__
#define __C_CSERIALPORT_H__

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

enum OperateMode
{
    AsynchronousOperate,
    SynchronousOperate
};

enum BaudRate
{
    BaudRate110 = 110,
    BaudRate300 = 300,
    BaudRate600 = 600,
    BaudRate1200 = 1200,
    BaudRate2400 = 2400,
    BaudRate4800 = 4800,
    BaudRate9600 = 9600,
    BaudRate14400 = 14400,
    BaudRate19200 = 19200,
    BaudRate38400 = 38400,
    BaudRate56000 = 56000,
    BaudRate57600 = 57600,
    BaudRate115200 = 115200,
    BaudRate921600 = 921600
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

enum SerialPortError
{
    ErrorUnknown = -1,
    ErrorOK = 0,
    ErrorFail = 1,
    ErrorNotImplemented,
    ErrorInner,
    ErrorNullPointer,
    ErrorInvalidParam,
    ErrorAccessDenied,
    ErrorOutOfMemory,
    ErrorTimeout,

    ErrorNotInit,
    ErrorInitFailed,
    ErrorAlreadyExist,
    ErrorNotExist,
    ErrorAlreadyOpen,
    ErrorNotOpen,
    ErrorOpenFailed,
    ErrorCloseFailed,
    ErrorWriteFailed,
    ErrorReadFailed
};

#ifdef __cplusplus
extern "C"
{
#endif
    typedef void (*pFunReadEvent)(void * /*pSerialPort*/, const char * /*portName*/, unsigned int /*readBufferLen*/);

    C_DLL_EXPORT void CSerialPortAvailablePortInfos(struct SerialPortInfoArray *portInfoArray);

    C_DLL_EXPORT void CSerialPortAvailablePortInfosFree(struct SerialPortInfoArray *portInfoArray);

    C_DLL_EXPORT void *CSerialPortMalloc();

    C_DLL_EXPORT void CSerialPortFree(void *pSerialPort);

    C_DLL_EXPORT void CSerialPortInit(void *pSerialPort,
                           const char *portName,
                           int baudRate,
                           enum Parity parity,
                           enum DataBits dataBits,
                           enum StopBits stopbits,
                           enum FlowControl flowControl,
                           unsigned int readBufferSize);

    C_DLL_EXPORT void CSerialPortSetOperateMode(void *pSerialPort, enum OperateMode operateMode);

    C_DLL_EXPORT int CSerialPortOpen(void *pSerialPort);

    C_DLL_EXPORT void CSerialPortClose(void *pSerialPort);

    C_DLL_EXPORT int CSerialPortIsOpen(void *pSerialPort);

    C_DLL_EXPORT int CSerialPortConnectReadEvent(void *pSerialPort, pFunReadEvent pFun);

    C_DLL_EXPORT int CSerialPortDisconnectReadEvent(void *pSerialPort);

    C_DLL_EXPORT unsigned int CSerialPortGetReadBufferUsedLen(void *pSerialPort);

    C_DLL_EXPORT int CSerialPortReadData(void *pSerialPort, void *data, int size);

    C_DLL_EXPORT int CSerialPortReadAllData(void *pSerialPort, void *data);

    C_DLL_EXPORT int CSerialPortReadLineData(void *pSerialPort, void *data, int size);

    C_DLL_EXPORT int CSerialPortWriteData(void *pSerialPort, const void *data, int size);

    C_DLL_EXPORT void CSerialPortSetDebugModel(void *pSerialPort, int isDebug);

    C_DLL_EXPORT void CSerialPortSetReadIntervalTimeout(void *pSerialPort, unsigned int msecs);

    C_DLL_EXPORT unsigned int CSerialPortGetReadIntervalTimeout(void *pSerialPort);

    C_DLL_EXPORT void CSerialPortSetMinByteReadNotify(void *pSerialPort, unsigned int minByteReadNotify);

    C_DLL_EXPORT int CSerialPortFlushBuffers(void *pSerialPort);

    C_DLL_EXPORT int CSerialPortFlushReadBuffers(void *pSerialPort);

    C_DLL_EXPORT int CSerialPortFlushWriteBuffers(void *pSerialPort);

    C_DLL_EXPORT int CSerialPortGetLastError(void *pSerialPort);

    C_DLL_EXPORT const char *CSerialPortGetLastErrorMsg(void *pSerialPort);

    C_DLL_EXPORT void CSerialPortClearError(void *pSerialPort);

    C_DLL_EXPORT void CSerialPortSetPortName(void *pSerialPort, const char *portName);

    C_DLL_EXPORT const char *CSerialPortGetPortName(void *pSerialPort);

    C_DLL_EXPORT void CSerialPortSetBaudRate(void *pSerialPort, int baudRate);

    C_DLL_EXPORT int CSerialPortGetBaudRate(void *pSerialPort);

    C_DLL_EXPORT void CSerialPortSetParity(void *pSerialPort, enum Parity parity);

    C_DLL_EXPORT enum Parity CSerialPortGetParity(void *pSerialPort);

    C_DLL_EXPORT void CSerialPortSetDataBits(void *pSerialPort, enum DataBits dataBits);

    C_DLL_EXPORT enum DataBits CSerialPortGetDataBits(void *pSerialPort);

    C_DLL_EXPORT void CSerialPortSetStopBits(void *pSerialPort, enum StopBits stopbits);

    C_DLL_EXPORT enum StopBits CSerialPortGetStopBits(void *pSerialPort);

    C_DLL_EXPORT void CSerialPortSetFlowControl(void *pSerialPort, enum FlowControl flowControl);

    C_DLL_EXPORT enum FlowControl CSerialPortGetFlowControl(void *pSerialPort);

    C_DLL_EXPORT void CSerialPortSetReadBufferSize(void *pSerialPort, unsigned int size);

    C_DLL_EXPORT unsigned int CSerialPortGetReadBufferSize(void *pSerialPort);

    C_DLL_EXPORT void CSerialPortSetDtr(void *pSerialPort, int set);

    C_DLL_EXPORT void CSerialPortSetRts(void *pSerialPort, int set);

    C_DLL_EXPORT const char *CSerialPortGetVersion(void *pSerialPort);

#ifdef __cplusplus
}
#endif

#endif // !__C_CSERIALPORT_H__