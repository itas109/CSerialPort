#include <iostream>
#include <chrono>

#include "CSerialPortWrapper.h"

#define WRAPPER_CLASS_NAME "CSerialPort"
#define DEFINE_FUNCTION(functionName) InstanceMethod(#functionName, &CSerialPortWrapper::functionName)

struct ReadDataType
{
    char *data;
    unsigned int readBufferLen;
};

Napi::FunctionReference CSerialPortWrapper::constructor;

Napi::Object CSerialPortWrapper::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func = DefineClass(env, WRAPPER_CLASS_NAME,
                                      {
                                          DEFINE_FUNCTION(onReadEvent),        //
                                          DEFINE_FUNCTION(availablePortInfos), //
                                          DEFINE_FUNCTION(init),               //
                                          DEFINE_FUNCTION(init2),              //
                                          DEFINE_FUNCTION(open),               //
                                          DEFINE_FUNCTION(close),              //
                                          DEFINE_FUNCTION(isOpen),             //
                                          DEFINE_FUNCTION(writeData),          //
                                          DEFINE_FUNCTION(readData),           //
                                          DEFINE_FUNCTION(readAllData),        //
                                          DEFINE_FUNCTION(setDtr),             //
                                          DEFINE_FUNCTION(setRts),             //
                                          DEFINE_FUNCTION(getVersion),         //
                                          DEFINE_FUNCTION(getLastError),       //
                                          DEFINE_FUNCTION(getLastErrorMsg)     //
                                      });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set(WRAPPER_CLASS_NAME, func);
    return exports;
}

CSerialPortWrapper::CSerialPortWrapper(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<CSerialPortWrapper>(info)
{
    m_tsfn = nullptr;
    m_isThreadRunning = false;

    m_serialPort.setOperateMode(itas109::SynchronousOperate);
}

CSerialPortWrapper::~CSerialPortWrapper()
{
    m_isThreadRunning = false;

    if (m_thread.joinable())
    {
        m_thread.join();
    }

    if (m_tsfn)
    {
        m_tsfn.Release();
    }
}

void CSerialPortWrapper::onReadEvent(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (m_isThreadRunning)
    {
        return;
    }

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsFunction())
    {
        Napi::TypeError::New(env, "callback must be function").ThrowAsJavaScriptException();
        return;
    }

    Napi::Function callback = info[0].As<Napi::Function>();

    m_tsfn = Napi::ThreadSafeFunction::New(env, callback, "ThreadSafeFun", 0, 1);

    m_isThreadRunning = true;
    m_thread = std::thread(
        [this]()
        {
            uint32_t count = 0;
            while (m_isThreadRunning)
            {
                int len = m_serialPort.getReadBufferUsedLen();
                if (len > 0)
                {
                    char *readData = new char[len];
                    int realLen = m_serialPort.readData(readData, len);
                    if (realLen > 0)
                    {

                        ReadDataType *dataType = new ReadDataType();
                        dataType->data = readData;
                        dataType->readBufferLen = realLen;
                        auto callback = [this](Napi::Env env, Napi::Function jsCallback, ReadDataType *dataType)
                        {
                            // Electron: External buffers are not allowed
                            // https://github.com/nodejs/node-addon-api/blob/main/doc/external_buffer.md
                            // https://github.com/electron/electron/issues/35801
                            jsCallback.Call({Napi::Buffer<char>::NewOrCopy(env, dataType->data, dataType->readBufferLen)});

                            if (dataType->data)
                            {
                                delete[] dataType->data;
                            }
                            if (dataType)
                            {
                                delete dataType;
                            }
                        };
                        m_tsfn.BlockingCall(dataType, callback);
                    }
                }
            }
        });
}

Napi::Value CSerialPortWrapper::availablePortInfos(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    std::vector<SerialPortInfo> portInfoList = CSerialPortInfo::availablePortInfos();

    Napi::Array array = Napi::Array::New(env, portInfoList.size());
    for (uint32_t i = 0; i < portInfoList.size(); i++)
    {
        SerialPortInfo serialPortInfo = portInfoList.at(i);
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("portName", Napi::String::New(env, serialPortInfo.portName));
        obj.Set("description", Napi::String::New(env, serialPortInfo.description));
        obj.Set("hardwareId", Napi::String::New(env, serialPortInfo.hardwareId));
        array.Set(i, obj);
    }

    return array;
}

void CSerialPortWrapper::init(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "portName must be string").ThrowAsJavaScriptException();
        return;
    }

    Napi::String portName = info[0].As<Napi::String>();

    m_serialPort.init(portName.Utf8Value().c_str());
}

void CSerialPortWrapper::init2(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "portName must be string").ThrowAsJavaScriptException();
        return;
    }
    Napi::String portName = info[0].As<Napi::String>();

    int baudRate = info[1].IsString() ? info[1].As<Napi::String>().ToNumber().Int32Value() : info[1].IsNumber() ? info[1].As<Napi::Number>().Int32Value() : 9600;

    m_serialPort.init(portName.Utf8Value().c_str(), baudRate);
}

Napi::Value CSerialPortWrapper::open(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    Napi::String portName = info[0].As<Napi::String>();

    return Napi::Boolean::New(env, m_serialPort.open());
}

void CSerialPortWrapper::close(const Napi::CallbackInfo &info)
{
    m_serialPort.close();
}

Napi::Value CSerialPortWrapper::isOpen(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, m_serialPort.isOpen());
}

Napi::Value CSerialPortWrapper::writeData(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    if (!info[0].IsBuffer())
    {
        Napi::TypeError::New(env, "first param must be Buffer").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }
    Napi::Buffer<char> buffer = info[0].As<Napi::Buffer<char>>();

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "second param must be number").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }
    int size = info[1].As<Napi::Number>().Int32Value();

    int len = m_serialPort.writeData(buffer.Data(), size);

    return Napi::Number::New(env, len);
}

Napi::Value CSerialPortWrapper::readData(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    if (!info[0].IsBuffer())
    {
        Napi::TypeError::New(env, "first param must be Buffer").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }
    Napi::Buffer<char> buffer = info[0].As<Napi::Buffer<char>>();

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "second param must be number").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }
    int size = info[1].As<Napi::Number>().Int32Value();

    char *data = buffer.Data();
    int len = m_serialPort.readData(data, size);

    return Napi::Number::New(env, len);
}

Napi::Value CSerialPortWrapper::readAllData(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }

    if (!info[0].IsBuffer())
    {
        Napi::TypeError::New(env, "first param must be Buffer").ThrowAsJavaScriptException();
        return Napi::Number::New(env, -1);
    }
    Napi::Buffer<char> buffer = info[0].As<Napi::Buffer<char>>();

    char *data = buffer.Data();
    int len = m_serialPort.readAllData(data);

    return Napi::Number::New(env, len);
}

void CSerialPortWrapper::setDtr(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsBoolean())
    {
        Napi::TypeError::New(env, "first param must be boolean").ThrowAsJavaScriptException();
        return;
    }

    bool isSet = info[0].As<Napi::Boolean>().Value();

    m_serialPort.setDtr(isSet);
}

void CSerialPortWrapper::setRts(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsBoolean())
    {
        Napi::TypeError::New(env, "first param must be boolean").ThrowAsJavaScriptException();
        return;
    }

    bool isSet = info[0].As<Napi::Boolean>().Value();

    m_serialPort.setRts(isSet);
}

Napi::Value CSerialPortWrapper::getVersion(const Napi::CallbackInfo &info)
{
    return Napi::String::New(info.Env(), m_serialPort.getVersion());
}

Napi::Value CSerialPortWrapper::getLastError(const Napi::CallbackInfo &info)
{
    return Napi::Number::New(info.Env(), m_serialPort.getLastError());
}

Napi::Value CSerialPortWrapper::getLastErrorMsg(const Napi::CallbackInfo &info)
{
    return Napi::String::New(info.Env(), m_serialPort.getLastErrorMsg());
}
