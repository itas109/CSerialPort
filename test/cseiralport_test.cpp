﻿// doctest
#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_NO_POSIX_SIGNALS // fixed SIGSTKSZ compile error on ubuntu 22
#include "doctest.h"

#include <iostream>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#define imsleep(microsecond) Sleep(microsecond) // ms
#else
#include <unistd.h>
#define imsleep(microsecond) usleep(1000 * microsecond) // ms
#endif

#include "CSerialPort/iutils.hpp"
#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
#include "CSerialPort/IProtocolParser.h" // optional for parser
#include "CSerialPortVirtual.h"
using namespace itas109;

// ReadEventNotify
#ifdef CSERIALPORT_CPP11
#include <mutex>
#include <condition_variable>
#else
#include "CSerialPort/ithread.hpp"
#endif

static char portName1[256] = {0};
static char portName2[256] = {0};

class ReadEventNotify
{
public:
    static ReadEventNotify &getInstance()
    {
        static ReadEventNotify instance;
        return instance;
    }

    void wait(char *portName, unsigned int &readBufferLen, unsigned int timeoutMS = 100)
    {
#ifdef CSERIALPORT_CPP11
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_cv.wait_for(lock, std::chrono::milliseconds(timeoutMS), [&]
                          { return m_readBufferLen > 0; }))
#else
        if (m_cv.timeWait(m_mutex, timeoutMS, m_readBufferLen > 0))
#endif
        {
            readBufferLen = m_readBufferLen;
            itas109::IUtils::strncpy(portName, m_portName, 256);
        }
        else
        {
            printf("wait %u timeout\n", timeoutMS);
        }
    }

    void notify(const char *portName, unsigned int readBufferLen)
    {
        {
#ifdef CSERIALPORT_CPP11
            std::unique_lock<std::mutex> lock(m_mutex);
#else
            IAutoLock lock(&m_mutex);
#endif
            m_readBufferLen = readBufferLen;
            itas109::IUtils::strncpy(m_portName, portName, 256);
        }
#ifdef CSERIALPORT_CPP11
        m_cv.notify_one();
#else
        m_cv.notifyOne();
#endif
    }

private:
    ReadEventNotify()
    {
        memset(m_portName, 0, 256);
        m_readBufferLen = 0;
    }

private:
#ifdef CSERIALPORT_CPP11
    std::condition_variable m_cv;
    std::mutex m_mutex;
#else
    IConditionVariable m_cv;
    IMutex m_mutex;
#endif

    char m_portName[256];
    unsigned int m_readBufferLen;
};

class MyListener : public CSerialPortListener, public CSerialPortHotPlugListener
{
public:
    MyListener() {};

    void onReadEvent(const char *portName, unsigned int readBufferLen)
    {
        ReadEventNotify::getInstance().notify(portName, readBufferLen);
    };

    void onHotPlugEvent(const char *portName, int isAdd)
    {
        printf("portName: %s, isAdded: %d\n", portName, isAdd);
    }
};

class ProtocolEventNotify
{
public:
    static ProtocolEventNotify &getInstance()
    {
        static ProtocolEventNotify instance;
        return instance;
    }

    void wait(std::vector<IProtocolResult> &results, unsigned int timeoutMS = 100)
    {
#ifdef CSERIALPORT_CPP11
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_cv.wait_for(lock, std::chrono::milliseconds(timeoutMS), [&]
                          { return m_results.size() > 0; }))
#else
        if (m_cv.timeWait(m_mutex, timeoutMS, m_results.size() > 0))
#endif
        {
#ifdef CSERIALPORT_CPP11
            results = std::move(m_results);
#else
            results = m_results;
#endif
        }
        else
        {
            printf("wait %u timeout\n", timeoutMS);
        }
    }

    void notify(std::vector<IProtocolResult> &results)
    {
        {
#ifdef CSERIALPORT_CPP11
            std::unique_lock<std::mutex> lock(m_mutex);
            m_results = std::move(results);
#else
            IAutoLock lock(&m_mutex);
            m_results = results;
#endif
        }

#ifdef CSERIALPORT_CPP11
        m_cv.notify_one();
#else
        m_cv.notifyOne();
#endif
    }

private:
    ProtocolEventNotify()
    {
    }

private:
#ifdef CSERIALPORT_CPP11
    std::condition_variable m_cv;
    std::mutex m_mutex;
#else
    IConditionVariable m_cv;
    IMutex m_mutex;
#endif

    std::vector<IProtocolResult> m_results;
};

class CommonProtocolParser : public LengthFieldBasedProtocolParser
{
public:
    CommonProtocolParser(
        unsigned int headerFieldLength, // 0 1 2
        unsigned char header[2],
        unsigned int lengthFieldOffset,
        unsigned int lengthFieldLength,
        unsigned int lengthAdjustment,
        unsigned int lengthFieldMaxValue,
        unsigned int checkCodeFieldLength, // 0 1 2
        unsigned int footerFieldLength,    // 0 1 2
        unsigned char footer[2])
        : LengthFieldBasedProtocolParser(headerFieldLength, header, lengthFieldOffset, lengthFieldLength, lengthAdjustment, lengthFieldMaxValue, checkCodeFieldLength, footerFieldLength, footer)
    {
    }

    bool isCheckCodeValid(const unsigned char *data, unsigned int size)
    {
        static const unsigned short CRC16ModbusTable[256] = {
            0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 0xC601, 0x06C0, 0x0780, 0xC741,
            0x0500, 0xC5C1, 0xC481, 0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
            0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841, 0xD801, 0x18C0, 0x1980, 0xD941,
            0x1B00, 0xDBC1, 0xDA81, 0x1A40, 0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
            0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641, 0xD201, 0x12C0, 0x1380, 0xD341,
            0x1100, 0xD1C1, 0xD081, 0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
            0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441, 0x3C00, 0xFCC1, 0xFD81, 0x3D40,
            0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
            0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41,
            0x2D00, 0xEDC1, 0xEC81, 0x2C40, 0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
            0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041, 0xA001, 0x60C0, 0x6180, 0xA141,
            0x6300, 0xA3C1, 0xA281, 0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
            0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41, 0xAA01, 0x6AC0, 0x6B80, 0xAB41,
            0x6900, 0xA9C1, 0xA881, 0x6840, 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
            0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541,
            0x7700, 0xB7C1, 0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
            0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241, 0x9601, 0x56C0, 0x5780, 0x9741,
            0x5500, 0x95C1, 0x9481, 0x5440, 0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
            0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841, 0x8801, 0x48C0, 0x4980, 0x8941,
            0x4B00, 0x8BC1, 0x8A81, 0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
            0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641, 0x8201, 0x42C0, 0x4380, 0x8341,
            0x4100, 0x81C1, 0x8081, 0x4040};

        unsigned short receivedCheckCode = (data[size - 2] << 8) | data[size - 1];
        unsigned short calcCheckCode = 0xFFFF;
        size -= 2;
        while (size--)
        {
            calcCheckCode = (calcCheckCode >> 8) ^ CRC16ModbusTable[(calcCheckCode ^ *data++) & 0xFF];
        }

        return calcCheckCode == receivedCheckCode;
    }

    void onProtocolEvent(std::vector<IProtocolResult> &results)
    {
        char hexStr[200];
        for (size_t i = 0; i < results.size(); ++i)
        {
            const IProtocolResult result = results.at(i);
            printf("parse protocol result. len: %d, hex(top100): %s\n", result.len, itas109::IUtils::charToHexStr(hexStr, (char *)result.data, result.len > 100 ? 100 : result.len));
        }
        ProtocolEventNotify::getInstance().notify(results);
    }
};

class ATCommandProtocolParser : public DelimiterBasedProtocolParser
{
public:
    ATCommandProtocolParser(
        unsigned int headerFieldLength, // 0 1 2
        unsigned char header[2],
        unsigned int footerFieldLength,    // 1 2
        unsigned char footer[2])
        : DelimiterBasedProtocolParser(headerFieldLength, header, footerFieldLength, footer)
    {
    }

    void onProtocolEvent(std::vector<IProtocolResult> &results)
    {
        char hexStr[200];
        for (size_t i = 0; i < results.size(); ++i)
        {
            const IProtocolResult result = results.at(i);
            printf("parse protocol result. str: %s, len: %d, hex(top100): %s\n", (char*)result.data, result.len, itas109::IUtils::charToHexStr(hexStr, (char *)result.data, result.len > 100 ? 100 : result.len));
        }
        ProtocolEventNotify::getInstance().notify(results);
    }
};

inline unsigned char hexToByte(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }

    if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    return 0xFF;
};

class NMEA0183CommandProtocolParser : public DelimiterBasedProtocolParser
{
public:
    NMEA0183CommandProtocolParser(
        unsigned int headerFieldLength, // 0 1 2
        unsigned char header[2],
        unsigned int footerFieldLength, // 1 2
        unsigned char footer[2])
        : DelimiterBasedProtocolParser(headerFieldLength, header, footerFieldLength, footer)
    {
    }

    bool isCheckCodeValid(const unsigned char *data, unsigned int size)
    {
        if (size < 6 || data[0] != '$') // $*00\r\n
        {
            return false;
        }

        // XOR of all bytes between $ and *
        const unsigned char *ptr = data + 1;
        unsigned char checksum = 0;
        while ('*' != *ptr && '\r' != *ptr && '\0' != *ptr)
        {
            checksum ^= *ptr++;
        }

        unsigned char high = hexToByte(*(ptr+1));
        unsigned char low = hexToByte(*(ptr+2));
        if (high == 0xFF || low == 0xFF)
        {
            return false;
        }
        unsigned char receivedCheckCode = (high << 4) | low;

        return checksum == receivedCheckCode;
    }

    void onProtocolEvent(std::vector<IProtocolResult> &results)
    {
        for (size_t i = 0; i < results.size(); ++i)
        {
            const IProtocolResult result = results.at(i);
            printf("parse protocol result. str: %s, len: %d\n", (char *)result.data, result.len);
        }
        ProtocolEventNotify::getInstance().notify(results);
    }
};


class CSerialPortTests
{
public:
    CSerialPortTests() // run before test
    {
        // std::cout << "CSerialPort Version : " << m_serialport1.getVersion() << std::endl;

        m_availablePortsInfoVector = CSerialPortInfo::availablePortInfos();

        REQUIRE_MESSAGE(m_availablePortsInfoVector.size() > 0, "No valid port");
    }

    ~CSerialPortTests() // run after test
    {
        if (m_serialport1.isOpen())
        {
            m_serialport1.close();
        }

        if (m_serialport2.isOpen())
        {
            m_serialport2.close();
        }

        // 防止太快导致串口没有被释放
        imsleep(50);
    }

protected:
    CSerialPort m_serialport1;
    CSerialPort m_serialport2;
    std::vector<SerialPortInfo> m_availablePortsInfoVector;
};

// #0_1 是否包含已创建的虚拟串口
TEST_CASE_FIXTURE(CSerialPortTests, "isContainsVirtualPair_0_1")
{
    std::cout << "Serial Port available Count : " << m_availablePortsInfoVector.size() << std::endl;
    int virtualSerialPortCount = 0;
    for (size_t i = 0; i < m_availablePortsInfoVector.size(); i++)
    {
        if (0 == strcmp(m_availablePortsInfoVector[i].portName, portName1) || 0 == strcmp(m_availablePortsInfoVector[i].portName, portName2))
        {
            ++virtualSerialPortCount;
        }
    }

    CHECK_EQ(virtualSerialPortCount, 2);
}

// #1_1 open port 打开未占用的串口
TEST_CASE_FIXTURE(CSerialPortTests, "open_1_1")
{
    m_serialport1.init(portName1);
    CHECK_MESSAGE(m_serialport1.open(), portName1 << " open error");
}

#ifdef _WIN32
// #1_2 open port 打开占用的串口
TEST_CASE_FIXTURE(CSerialPortTests, "open_1_2")
{
    m_serialport2.init(portName1);
    REQUIRE_MESSAGE(m_serialport2.open(), portName1 << " open error");

    m_serialport1.init(portName1);
    CHECK_FALSE_MESSAGE(m_serialport1.open(), portName1 << " is used,but open ok");
}
#endif

// #1_3 multiple open port 同时打开多个串口
TEST_CASE_FIXTURE(CSerialPortTests, "open_1_3")
{
    REQUIRE_MESSAGE(m_availablePortsInfoVector.size() > 1, "port count < 2");

    m_serialport1.init(portName1);
    m_serialport2.init(portName2);

    CHECK_MESSAGE(m_serialport1.open(), portName1 << " open error");
    CHECK_MESSAGE(m_serialport2.open(), portName2 << " open error");
}

// #2_1 未打开的串口,isOpen()是否与open()返回值一致
TEST_CASE_FIXTURE(CSerialPortTests, "isOpen_2_1")
{
    m_serialport1.init(portName1);
    bool isOpen = m_serialport1.open();
    CHECK_MESSAGE(isOpen == m_serialport1.isOpen(), portName1 << " function isOpen() isOpen status not equal function open()");
}

// #2_2 被占用的串口,isOpen()是否与open()返回值一致
TEST_CASE_FIXTURE(CSerialPortTests, "isOpen_2_2")
{
    m_serialport2.init(portName1);
    REQUIRE_MESSAGE(m_serialport2.open(), portName1 << " open error");

    m_serialport1.init(portName1);
    bool isOpen = m_serialport1.open();
    CHECK_MESSAGE(isOpen == m_serialport1.isOpen(), portName1 << " function isOpen() isOpen status not equal function open()");
}

// #3_1 open port and close 打开并关闭串口
TEST_CASE_FIXTURE(CSerialPortTests, "open_close_3_1")
{
    m_serialport1.init(portName1);
    REQUIRE_MESSAGE(m_serialport1.open(), portName1 << " open error");

    m_serialport1.close();
    CHECK_FALSE_MESSAGE(m_serialport1.isOpen(), portName1 << " open error");
}

// #3_2 open close and then open close 打开并关闭串口，再次打开并关闭串口
TEST_CASE_FIXTURE(CSerialPortTests, "open_close_3_2")
{
    m_serialport1.init(portName1);
    m_serialport1.open();
    REQUIRE_MESSAGE(m_serialport1.isOpen(), portName1 << " open error");
    m_serialport1.close();
    CHECK_FALSE_MESSAGE(m_serialport1.isOpen(), portName1 << " close error");

    m_serialport1.open();
    CHECK_MESSAGE(m_serialport1.isOpen(), portName1 << " open again error");
    m_serialport1.close();
    CHECK_FALSE_MESSAGE(m_serialport1.isOpen(), portName1 << " close again error");
}

// #3_3 打开并关闭串口(9600,N,1)，切换串口，再次打开串口(9600,N,1)
TEST_CASE_FIXTURE(CSerialPortTests, "open_close_3_3")
{
    REQUIRE_MESSAGE(m_availablePortsInfoVector.size() > 1, "port count < 2");

    m_serialport1.init(portName1);
    m_serialport1.open();
    CHECK_MESSAGE(m_serialport1.isOpen(), portName1 << " open error");
    m_serialport1.close();
    CHECK_FALSE_MESSAGE(m_serialport1.isOpen(), portName1 << " close error");

    m_serialport1.init(portName2);
    m_serialport1.open();
    CHECK_MESSAGE(m_serialport1.isOpen(), portName2 << " open again error");
    m_serialport1.close();
    CHECK_FALSE_MESSAGE(m_serialport1.isOpen(), portName2 << " close again error");
}

// #3_4 打开并关闭串口(9600,N,1)，切换波特率，再次打开串口(115200,N,1)
TEST_CASE_FIXTURE(CSerialPortTests, "open_close_3_4")
{
    m_serialport1.init(portName1, 9600);
    m_serialport1.open();
    CHECK_MESSAGE(m_serialport1.isOpen(), portName1 << " open error");
    m_serialport1.close();
    CHECK_FALSE_MESSAGE(m_serialport1.isOpen(), portName1 << " close error");

    m_serialport1.init(portName1, 115200);
    m_serialport1.open();
    CHECK_MESSAGE(m_serialport1.isOpen(), portName1 << " open again error");
    m_serialport1.close();
    CHECK_FALSE_MESSAGE(m_serialport1.isOpen(), portName1 << " close again error");
}

// #4_1 read sync 同步读
TEST_CASE_FIXTURE(CSerialPortTests, "read_sync_4_1")
{
    REQUIRE_MESSAGE(m_availablePortsInfoVector.size() > 1, "port count < 2");

    m_serialport1.init(portName1);
    m_serialport2.init(portName2);

    m_serialport1.setOperateMode(itas109::SynchronousOperate);
    m_serialport2.setOperateMode(itas109::SynchronousOperate);

    CHECK_EQ(true, m_serialport1.open());
    CHECK_EQ(true, m_serialport2.open());

    const char *writeData = "itas109";
    m_serialport1.writeData(writeData, 7);

    imsleep(10); // TODO: wait sync event

    char readData[1024] = {0};
    m_serialport2.readData(readData, 7);

    CHECK(0 == strcmp(writeData, readData));
}

// #5_1 read async 异步读
TEST_CASE_FIXTURE(CSerialPortTests, "read_async_5_1")
{
    REQUIRE_MESSAGE(m_availablePortsInfoVector.size() > 1, "port count < 2");

    MyListener listener;
    m_serialport2.connectReadEvent(&listener);

    m_serialport1.init(portName1);
    m_serialport2.init(portName2);

    CHECK_EQ(true, m_serialport1.open());
    CHECK_EQ(true, m_serialport2.open());

    const char *writeData = "itas109";
    int writeLen = strlen(writeData);
    m_serialport1.writeData(writeData, writeLen);

    // wait read event
    char portNameNotify[256] = {0};
    unsigned int readBufferLen = 0;
    ReadEventNotify::getInstance().wait(portNameNotify, readBufferLen, 1000);
    CHECK(0 == strcmp(portNameNotify, portName2));
    CHECK_EQ(writeLen, readBufferLen);

    // imsleep(50); // TODO: wait async event

    int readLen = m_serialport2.getReadBufferUsedLen();
    CHECK_EQ(writeLen, readLen);

    char readData[1024] = {0};
    m_serialport2.readData(readData, 7);

    CHECK(0 == strcmp(writeData, readData));
}

// #6_1 common protocol parse 通用协议帧解析
TEST_CASE_FIXTURE(CSerialPortTests, "common_protocol_parse_6_1")
{
    REQUIRE_MESSAGE(m_availablePortsInfoVector.size() > 1, "port count < 2");

    unsigned char header[2] = {0xEB, 0x90};
    unsigned char footer[2] = {};
    CommonProtocolParser parser(2, header, 3, 2, 2, 249, 2, 0, footer);
    m_serialport2.setProtocolParser(&parser);

    m_serialport1.init(portName1);
    m_serialport2.init(portName2);

    CHECK_EQ(true, m_serialport1.open());
    CHECK_EQ(true, m_serialport2.open());

    //  空帧
    {
        unsigned char writeData[7] = {0xEB, 0x90, 0x00, 0x00, 0x00, 0x17, 0x2D};
        int writeLen = sizeof(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(1 == result.size());
        CHECK(0 == memcmp(writeData, result[0].data, writeLen));
        CHECK_EQ(writeLen, result[0].len);
    }

    // 标准帧
    {
        unsigned char writeData[10] = {0xEB, 0x90, 0x00, 0x00, 0x03, 0x30, 0x31, 0x32, 0xF7, 0x0B};
        int writeLen = sizeof(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(1 == result.size());
        CHECK(0 == memcmp(writeData, result[0].data, writeLen));
        CHECK_EQ(writeLen, result[0].len);
    }

    // 多帧
    {
        unsigned char writeData[18] = {0xEB, 0x90, 0x00, 0x00, 0x03, 0x30, 0x31, 0x32, 0xF7, 0x0B, 0xEB, 0x90, 0x00, 0x00, 0x01, 0x30, 0x99, 0xD6};
        int writeLen = sizeof(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(2 == result.size());
        CHECK(0 == memcmp(writeData, result[0].data, 10));
        CHECK_EQ(10, result[0].len);
        CHECK(0 == memcmp(writeData + 10, result[1].data, 8));
        CHECK_EQ(8, result[1].len);
    }

    // 半帧拼接 - 帧头
    {
        unsigned char writeData1[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0xEB};
        int writeLen1 = sizeof(writeData1);
        unsigned char writeData2[9] = {0x90, 0x00, 0x00, 0x03, 0x30, 0x31, 0x32, 0xF7, 0x0B};
        int writeLen2 = sizeof(writeData2);
        unsigned char writeData[10] = {0xEB, 0x90, 0x00, 0x00, 0x03, 0x30, 0x31, 0x32, 0xF7, 0x0B};
        int writeLen = sizeof(writeData);
        m_serialport1.writeData(writeData1, writeLen1); // 前半帧

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(0 == result.size());

        m_serialport1.writeData(writeData2, writeLen2); // 后半帧
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(1 == result.size());
        CHECK(0 == memcmp(writeData, result[0].data, writeLen));
        CHECK_EQ(writeLen, result[0].len);
    }

    // 半帧拼接 - 长度
    {
        unsigned char writeData1[5] = {0xEB, 0x90, 0x00, 0x00, 0x03};
        int writeLen1 = sizeof(writeData1);
        unsigned char writeData2[5] = {0x30, 0x31, 0x32, 0xF7, 0x0B};
        int writeLen2 = sizeof(writeData2);
        unsigned char writeData[10] = {0xEB, 0x90, 0x00, 0x00, 0x03, 0x30, 0x31, 0x32, 0xF7, 0x0B};
        int writeLen = sizeof(writeData);
        m_serialport1.writeData(writeData1, writeLen1); // 前半帧

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(0 == result.size());

        m_serialport1.writeData(writeData2, writeLen2); // 后半帧
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(1 == result.size());
        CHECK(0 == memcmp(writeData, result[0].data, writeLen));
        CHECK_EQ(writeLen, result[0].len);
    }

    // 多余帧头
    {
        unsigned char writeData[12] = {0xEB, 0xEB, 0x90, 0x00, 0x00, 0x03, 0x30, 0x31, 0x32, 0xF7, 0x0B};
        int writeLen = sizeof(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(1 == result.size());
        CHECK(0 == memcmp(writeData + 1, result[0].data, 10));
        CHECK_EQ(10, result[0].len);
    }

    // 多余帧头无效长度
    {
        unsigned char writeData[13] = {0xEB, 0x90, 0xEB, 0x90, 0x00, 0x00, 0x03, 0x30, 0x31, 0x32, 0xF7, 0x0B};
        int writeLen = sizeof(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(1 == result.size());
        CHECK(0 == memcmp(writeData + 2, result[0].data, 10));
        CHECK_EQ(10, result[0].len);
    }

    // 无帧头
    {
        unsigned char writeData[7] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
        int writeLen = sizeof(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(0 == result.size());
    }

    // CRC错误
    {
        unsigned char writeData[7] = {0xEB, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00};
        int writeLen = sizeof(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(0 == result.size());
    }
}

// #6_2 at protocol parse AT指令协议帧解析
TEST_CASE_FIXTURE(CSerialPortTests, "at_protocol_parse_6_2")
{
    REQUIRE_MESSAGE(m_availablePortsInfoVector.size() > 1, "port count < 2");

    unsigned char header[2] = {'A', 'T'};   // AT
    unsigned char footer[2] = {'\r', '\n'}; // \r\n
    ATCommandProtocolParser parser(2, header, 2, footer);
    m_serialport2.setProtocolParser(&parser);

    m_serialport1.init(portName1);
    m_serialport2.init(portName2);

    CHECK_EQ(true, m_serialport1.open());
    CHECK_EQ(true, m_serialport2.open());

    //  空帧
    {
        unsigned char writeData[4] = {0x41, 0x54, 0x0D, 0x0A}; // AT\r\n
        int writeLen = sizeof(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(1 == result.size());
        CHECK(0 == memcmp(writeData, result[0].data, writeLen));
        CHECK_EQ(writeLen, result[0].len);
    }

    // 标准帧
    {
        unsigned char writeData[10] = {0x41, 0x54, 0x2B, 0x42, 0x41, 0x55, 0x44, 0x34, 0x0D, 0x0A}; // AT+BAUD4\r\n
        int writeLen = sizeof(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(1 == result.size());
        CHECK(0 == memcmp(writeData, result[0].data, writeLen));
        CHECK_EQ(writeLen, result[0].len);
    }

    // 多帧
    {
        unsigned char writeData[14] = {0x41, 0x54, 0x2B, 0x42, 0x41, 0x55, 0x44, 0x34, 0x0D, 0x0A,0x41, 0x54, 0x0D, 0x0A};
        int writeLen = sizeof(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(2 == result.size());
        CHECK(0 == memcmp(writeData, result[0].data, 10));
        CHECK_EQ(10, result[0].len);
        CHECK(0 == memcmp(writeData + 10, result[1].data, 4));
        CHECK_EQ(4, result[1].len);
    }

    // 半帧拼接 - 帧头
    {
        unsigned char writeData1[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x41};
        int writeLen1 = sizeof(writeData1);
        unsigned char writeData2[9] = {0x54, 0x2B, 0x42, 0x41, 0x55, 0x44, 0x34, 0x0D, 0x0A};
        int writeLen2 = sizeof(writeData2);
        unsigned char writeData[10] = {0x41, 0x54, 0x2B, 0x42, 0x41, 0x55, 0x44, 0x34, 0x0D, 0x0A};
        int writeLen = sizeof(writeData);
        m_serialport1.writeData(writeData1, writeLen1); // 前半帧

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(0 == result.size());

        m_serialport1.writeData(writeData2, writeLen2); // 后半帧
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(1 == result.size());
        CHECK(0 == memcmp(writeData, result[0].data, writeLen));
        CHECK_EQ(writeLen, result[0].len);
    }

    // 半帧拼接 - 帧尾
    {
        unsigned char writeData1[9] = {0x41, 0x54, 0x2B, 0x42, 0x41, 0x55, 0x44, 0x34, 0x0D};
        int writeLen1 = sizeof(writeData1);
        unsigned char writeData2[1] = {0x0A};
        int writeLen2 = sizeof(writeData2);
        unsigned char writeData[10] = {0x41, 0x54, 0x2B, 0x42, 0x41, 0x55, 0x44, 0x34, 0x0D, 0x0A};
        int writeLen = sizeof(writeData);
        m_serialport1.writeData(writeData1, writeLen1); // 前半帧

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(0 == result.size());

        m_serialport1.writeData(writeData2, writeLen2); // 后半帧
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(1 == result.size());
        CHECK(0 == memcmp(writeData, result[0].data, writeLen));
        CHECK_EQ(writeLen, result[0].len);
    }

    // 多余帧头
    {
        unsigned char writeData[11] = {0x41, 0x41, 0x54, 0x2B, 0x42, 0x41, 0x55, 0x44, 0x34, 0x0D, 0x0A};
        int writeLen = sizeof(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(1 == result.size());
        CHECK(0 == memcmp(writeData + 1, result[0].data, 10));
        CHECK_EQ(10, result[0].len);
    }

    // 无帧头
    {
        unsigned char writeData[7] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
        int writeLen = sizeof(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(0 == result.size());
    }
}

// #6_3 gps(NMEA0183) protocol parse AT指令协议帧解析
TEST_CASE_FIXTURE(CSerialPortTests, "nmea0183_protocol_parse_6_3")
{
    REQUIRE_MESSAGE(m_availablePortsInfoVector.size() > 1, "port count < 2");

    unsigned char header[2] = {'$', 0x00};  // $
    unsigned char footer[2] = {'\r', '\n'}; // \r\n
    NMEA0183CommandProtocolParser parser(1, header, 2, footer);
    m_serialport2.setProtocolParser(&parser);

    m_serialport1.init(portName1);
    m_serialport2.init(portName2);

    CHECK_EQ(true, m_serialport1.open());
    CHECK_EQ(true, m_serialport2.open());

    //  空帧
    {
        const char *writeData = "$*00\r\n";
        int writeLen = strlen(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(1 == result.size());
        CHECK(0 == memcmp(writeData, result[0].data, writeLen));
        CHECK_EQ(writeLen, result[0].len);
    }

    // 标准帧
    {
        const char *writeData = "$GPGGA,092750.00,3723.24,N,12158.34,W,1,10,1.0,9.0,M,,*26\r\n";
        int writeLen = strlen(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(1 == result.size());
        CHECK(0 == memcmp(writeData, result[0].data, writeLen));
        CHECK_EQ(writeLen, result[0].len);
    }

    // 多帧
    {
        const char *writeData = "$GPVTG,,*52\r\n$GPGGA,092750.00,3723.24,N,12158.34,W,1,10,1.0,9.0,M,,*26\r\n";
        int writeLen = strlen(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(2 == result.size());
        CHECK(0 == memcmp(writeData, result[0].data, 13));
        CHECK_EQ(13, result[0].len);
        CHECK(0 == memcmp(writeData + 13, result[1].data, 59));
        CHECK_EQ(59, result[1].len);
    }

    // 半帧拼接 - 数据
    {
        const char *writeData1 = "$GPGGA,092750.00,3723.24,N";
        int writeLen1 = strlen(writeData1);
        const char *writeData2 = ",12158.34,W,1,10,1.0,9.0,M,,*26\r\n";
        int writeLen2 = strlen(writeData2);
        const char *writeData = "$GPGGA,092750.00,3723.24,N,12158.34,W,1,10,1.0,9.0,M,,*26\r\n";
        int writeLen = strlen(writeData);
        m_serialport1.writeData(writeData1, writeLen1); // 前半帧

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(0 == result.size());

        m_serialport1.writeData(writeData2, writeLen2); // 后半帧
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(1 == result.size());
        CHECK(0 == memcmp(writeData, result[0].data, writeLen));
        CHECK_EQ(writeLen, result[0].len);
    }

    // 半帧拼接 - 帧尾
    {
        const char *writeData1 = "$GPGGA,092750.00,3723.24,N,12158.34,W,1,10,1.0,9.0,M,,*26\r";
        int writeLen1 = strlen(writeData1);
        const char *writeData2 = "\n";
        int writeLen2 = strlen(writeData2);
        const char *writeData = "$GPGGA,092750.00,3723.24,N,12158.34,W,1,10,1.0,9.0,M,,*26\r\n";
        int writeLen = strlen(writeData);
        m_serialport1.writeData(writeData1, writeLen1); // 前半帧

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(0 == result.size());

        m_serialport1.writeData(writeData2, writeLen2); // 后半帧
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(1 == result.size());
        CHECK(0 == memcmp(writeData, result[0].data, writeLen));
        CHECK_EQ(writeLen, result[0].len);
    }

    // 多余帧头
    {
        const char *writeData = "$$GPGGA,092750.00,3723.24,N,12158.34,W,1,10,1.0,9.0,M,,*26\r\n";
        int writeLen = strlen(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(1 == result.size());
        CHECK(0 == memcmp(writeData + 1, result[0].data, writeLen - 1));
        CHECK_EQ(writeLen - 1, result[0].len);
    }

    // 无帧头
    {
        unsigned char writeData[7] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
        int writeLen = sizeof(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(0 == result.size());
    }

    // 校验错误
    {
        const char *writeData = "$GPGGA,092750.00,3723.24,N,12158.34,W,1,10,1.0,9.0,M,,*00\r\n";
        int writeLen = strlen(writeData);
        m_serialport1.writeData(writeData, writeLen);

        // wait protocol event
        std::vector<IProtocolResult> result;
        ProtocolEventNotify::getInstance().wait(result, 2000);
        REQUIRE(0 == result.size());
    }
}


int main(int argc, char **argv)
{
#ifdef _WIN32
    itas109::IUtils::strncpy(portName1, "COM40", 256);
    itas109::IUtils::strncpy(portName2, "COM50", 256);
#endif

    bool isOk = CSerialPortVirtual::createPair(portName1, portName2);
    printf("create virtual pair %s - %s %s\n", portName1, portName2, isOk ? "success" : "failed");

    imsleep(100); // wait create pair

    doctest::Context context;
    context.applyCommandLine(argc, argv);
    context.setOption("duration", true);

    int res = context.run();

    isOk = CSerialPortVirtual::deletePair(portName1);
    isOk &= CSerialPortVirtual::deletePair(portName2);
    printf("delete virtual pair %s - %s %s\n", portName1, portName2, isOk ? "success" : "failed");

#ifdef _DEBUG
    printf("\nPress Enter To Continue...\n");
    (void)getchar();
#endif

    return 0;
}