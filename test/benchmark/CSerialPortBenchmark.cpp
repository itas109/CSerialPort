#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <string.h>

#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
using namespace itas109;

#define MAX_BUFFER_SIZE 200000 // maxBufferSize = 1073741824;// 2^30

const unsigned char sendData[256] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
    32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
    64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
    96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};

unsigned long long count = 0;

unsigned char writeIndex = 0xFF;

auto startReadTime = std::chrono::high_resolution_clock::now();

class MyListener : public CSerialPortListener
{
public:
    MyListener(CSerialPort *sp)
        : p_sp(sp){};

    void onReadEvent(const char *portName, unsigned int readBufferLen)
    {
        if (0 == count)
        {
            startReadTime = std::chrono::high_resolution_clock::now();
        }

        int recLen = p_sp->readData(data, readBufferLen);

        for (int i = 0; i < recLen; ++i)
        {
            printf("%llu, %u\n", ++count, (unsigned char)data[i]);
        }

        if (count >= MAX_BUFFER_SIZE)
        {
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - startReadTime);
            printf("time(ms), %lld", duration.count());
            exit(0);
        }
    };

private:
    CSerialPort *p_sp;
    unsigned char data[MAX_BUFFER_SIZE] = {0}; // 0x00 - 0xFF (0 - 255)
};

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Please run application with PortName(COM1 or /dev/ttyS0) and RunMode(0: client, 1: server)\nCSerialPortBenchmark COM1 0 > read.csv\nCSerialPortBenchmark COM2 1 > write.csv\n");
        printf("Press Enter To Continue");
        getchar();
        return 0;
    }

    CSerialPort sp;
    MyListener listener(&sp);

    const char *runMode = argv[2];

    sp.init(argv[1],               // windows:COM1 Linux:/dev/ttyS0
            itas109::BaudRate9600, // baudrate
            itas109::ParityNone,   // parity
            itas109::DataBits8,    // data bit
            itas109::StopOne,      // stop bit
            itas109::FlowNone,     // flow
            MAX_BUFFER_SIZE       // read buffer size - maxBufferSize = 1073741824;// 2^30
    );
    sp.setReadIntervalTimeout(0); // read interval timeout 0ms
    // sp.setOperateMode(itas109::SynchronousOperate);

    if (!sp.open())
    {
        printf("Open %s Failed. Code: %d, Message: %s\n", argv[1], sp.getLastError(), sp.getLastErrorMsg());
        return 0;
    }

    if (0 == strcmp(runMode, "0"))
    {
        // CSerialPortBenchmark COM1 0 > read.csv
        printf("readIndex, %s data\n", argv[1]);
        // connect for read
        sp.connectReadEvent(&listener);

        for (;;)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }
    else
    {
        // CSerialPortBenchmark COM2 1 > write.csv
        printf("writeIndex, %s data\n", argv[1]);
        auto start_time = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < MAX_BUFFER_SIZE; ++i)
        {
            sp.writeData(&sendData[++writeIndex], 1);
            printf("%llu, %u\n", ++count, (unsigned char)sendData[writeIndex]);
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        printf("time(ms), %lld", duration.count());
    }

    return 0;
}