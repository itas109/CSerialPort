#include "napi.h"
#include "CSerialPortWrapper.h"

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    CSerialPortWrapper::Init(env, exports);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)