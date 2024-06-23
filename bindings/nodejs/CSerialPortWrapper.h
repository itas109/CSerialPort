#include <napi.h>
#include <thread>

#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
using namespace itas109;

class CSerialPortWrapper : public Napi::ObjectWrap<CSerialPortWrapper>
{
public:
    CSerialPortWrapper(const Napi::CallbackInfo &info);
    ~CSerialPortWrapper();
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

private:
    static Napi::FunctionReference constructor;

    Napi::ThreadSafeFunction m_tsfn;
    std::thread m_thread;
    bool m_isThreadRunning;
    void onReadEvent(const Napi::CallbackInfo &info);

private:
    CSerialPort m_serialPort;

    Napi::Value availablePortInfos(const Napi::CallbackInfo &info);
    void init(const Napi::CallbackInfo &info);
    Napi::Value open(const Napi::CallbackInfo &info);
    void close(const Napi::CallbackInfo &info);
    Napi::Value isOpen(const Napi::CallbackInfo &info);
    Napi::Value writeData(const Napi::CallbackInfo &info);
    Napi::Value readData(const Napi::CallbackInfo &info);
    Napi::Value readAllData(const Napi::CallbackInfo &info);
    Napi::Value getVersion(const Napi::CallbackInfo &info);
};