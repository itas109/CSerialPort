#include <iostream>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#define imsleep(microsecond) Sleep(microsecond) // ms
#else
#include <unistd.h>
#define imsleep(microsecond) usleep(1000 * microsecond) // ms
#endif

#ifndef _WIN32
#include <thread> // std::thread
#endif

// ReadEventNotify
#include <condition_variable>
#include <thread>
#include <chrono>

#include "gtest/gtest.h"

#include "CSerialPort/iutils.hpp"
#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
#include "CSerialPortVirtual.h"
using namespace itas109;

static char portName1[256] = {0};
static char portName2[256] = {0};

class ReadEventNotify
{
public:
    static ReadEventNotify& getInstance()
    {
        static ReadEventNotify instance;
        return instance;
    }

    void wait(char *portName, unsigned int& readBufferLen, unsigned int timeoutMS = 100)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_cv.wait_for(lock, std::chrono::microseconds(timeoutMS), [&] { return m_readBufferLen > 0; }))
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
            std::lock_guard<std::mutex> lock(m_mutex);
            m_readBufferLen = readBufferLen;
            itas109::IUtils::strncpy(m_portName, portName, 256);
        }

        m_cv.notify_one();
    }

private:
    ReadEventNotify()
        : m_portName{0}
        , m_readBufferLen(0)
    {};

private:
    std::condition_variable m_cv;
    std::mutex m_mutex;

    char m_portName[256];
    unsigned int m_readBufferLen;
};

class MyListener : public CSerialPortListener, public CSerialPortHotPlugListener
{
public:
    MyListener() {};

    void onReadEvent(const char *portName, unsigned int readBufferLen) 
    {
        m_readEventNotify.notify(portName, readBufferLen);
    };

    void onHotPlugEvent(const char *portName, int isAdd)
    {
        printf("portName: %s, isAdded: %d\n", portName, isAdd);
    }

private:
    ReadEventNotify& m_readEventNotify = ReadEventNotify::getInstance();
};

class CSerialPortTests : public testing::Test
{
protected:
    virtual void SetUp() // run before test
    {
        // std::cout << "CSerialPort Version : " << m_serialport.getVersion() << std::endl;

        m_availablePortsInfoVector = CSerialPortInfo::availablePortInfos();

        ASSERT_TRUE(m_availablePortsInfoVector.size() > 0) << "No valid port";
    }

    virtual void TearDown() // run after test
    {
        if (m_serialport.isOpen())
        {
            m_serialport.close();
        }

        if (m_serialport2.isOpen())
        {
            m_serialport2.close();
        }

        // 防止太快导致串口没有被释放
        imsleep(10);
    }
    CSerialPort m_serialport;
    CSerialPort m_serialport2;
    std::vector<SerialPortInfo> m_availablePortsInfoVector;
    ReadEventNotify &m_readEventNotify = ReadEventNotify::getInstance();
};

// #0_1 是否包含已创建的虚拟串口
TEST_F(CSerialPortTests, isContainsVirtualPair_0_1)
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

    EXPECT_EQ(virtualSerialPortCount, 2);
}

// #1_1 open port 打开未占用的串口
TEST_F(CSerialPortTests, open_1_1)
{
    m_serialport.init(portName1);
    EXPECT_TRUE(m_serialport.open()) << portName1 << " open error, error code: " << m_serialport.getLastError();
}

// #1_2 open port 打开占用的串口
TEST_F(CSerialPortTests, open_1_2)
{
    m_serialport2.init(portName1);
    ASSERT_TRUE(m_serialport2.open()) << portName1 << " Used error";

    m_serialport.init(portName1);
    EXPECT_FALSE(m_serialport.open()) << portName1 << " is used,but open ok";
}

// #1_3 multiple open port 同时打开多个串口
TEST_F(CSerialPortTests, open_1_3)
{
    ASSERT_TRUE(m_availablePortsInfoVector.size() > 1) << "port count < 2";

    m_serialport.init(portName1);
    m_serialport2.init(portName2);

    EXPECT_TRUE(m_serialport.open()) << portName1 << " open error, error code: " << m_serialport.getLastError();
    EXPECT_TRUE(m_serialport2.open()) << portName2 << " open error, error code: " << m_serialport.getLastError();
}

// #2_1 未打开的串口,isOpen()是否与open()返回值一致
TEST_F(CSerialPortTests, isOpen_2_1)
{
    m_serialport.init(portName1);
    bool isOpen = m_serialport.open();
    EXPECT_TRUE(isOpen == m_serialport.isOpen()) << portName1 << " function isOpen() isOpen status not equal function open() ";
}

// #2_2 被占用的串口,isOpen()是否与open()返回值一致
TEST_F(CSerialPortTests, isOpen_2_2)
{
    m_serialport2.init(portName1);
    ASSERT_TRUE(m_serialport2.open()) << portName1 << " Used error";

    m_serialport.init(portName1);
    bool isOpen = m_serialport.open();
    EXPECT_TRUE(isOpen == m_serialport.isOpen()) << portName1 << " function isOpen() isOpen status not equal function open() ";
}

// #3_1 open port and close 打开并关闭串口
TEST_F(CSerialPortTests, open_close_3_1)
{
    m_serialport.init(portName1);
    EXPECT_TRUE(m_serialport.open()) << portName1 << " open error, error code: " << m_serialport.getLastError();

    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpen()) << portName1 << " close error, error code: " << m_serialport.getLastError();
}

// #3_2 open close and then open close 打开并关闭串口，再次打开并关闭串口
TEST_F(CSerialPortTests, open_close_3_2)
{
    m_serialport.init(portName1);
    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpen()) << portName1 << " open error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpen()) << portName1 << " close error, error code: " << m_serialport.getLastError();

    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpen()) << portName1 << " open again error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpen()) << portName1 << " close again error, error code: " << m_serialport.getLastError();
}

// #3_3 打开并关闭串口(9600,N,1)，切换串口，再次打开串口(9600,N,1)
TEST_F(CSerialPortTests, open_close_3_3)
{
    ASSERT_TRUE(m_availablePortsInfoVector.size() > 1) << "port count < 2";

    m_serialport.init(portName1);
    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpen()) << portName1 << " open error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpen()) << portName1 << " close error, error code: " << m_serialport.getLastError();

    m_serialport.init(portName2);
    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpen()) << portName2 << " open again error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpen()) << portName2 << " close again error, error code: " << m_serialport.getLastError();
}

// #3_4 打开并关闭串口(9600,N,1)，切换波特率，再次打开串口(115200,N,1)
TEST_F(CSerialPortTests, open_close_3_4)
{
    m_serialport.init(portName1, 9600);
    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpen()) << portName1 << " open error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpen()) << portName1 << " close error, error code: " << m_serialport.getLastError();

    m_serialport.init(portName1, 115200);
    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpen()) << portName1 << " open again error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpen()) << portName1 << " close again error, error code: " << m_serialport.getLastError();
}

// #4_1 read sync 同步读
TEST_F(CSerialPortTests, read_sync_4_1)
{
    ASSERT_TRUE(m_availablePortsInfoVector.size() > 1) << "port count < 2";

    m_serialport.init(portName1);
    m_serialport2.init(portName2);

    m_serialport.setOperateMode(itas109::SynchronousOperate);
    m_serialport2.setOperateMode(itas109::SynchronousOperate);

    m_serialport.open();
    m_serialport2.open();

    const char *writeData = "itas109";
    m_serialport.writeData(writeData, 7);

    imsleep(10); // TODO: wait sync event

    char readData[1024] = {0};
    m_serialport2.readData(readData, 7);

    EXPECT_STREQ(writeData, readData);
}

// #5_1 read async 同步读
TEST_F(CSerialPortTests, read_async_5_1)
{
    ASSERT_TRUE(m_availablePortsInfoVector.size() > 1) << "port count < 2";

    MyListener listener;
    m_serialport2.connectReadEvent(&listener);

    m_serialport.init(portName1);
    m_serialport2.init(portName2);

    m_serialport.open();
    m_serialport2.open();

    const char *writeData = "itas109";
    int writeLen = strlen(writeData);
    m_serialport.writeData(writeData, writeLen);

    // wait read event
    char portName[256] = {0};
    unsigned int readBufferLen = 0;
    m_readEventNotify.wait(portName, readBufferLen, 1000);
    EXPECT_STREQ(portName, portName2);
    EXPECT_EQ(writeLen, readBufferLen);

    // imsleep(50); // TODO: wait async event

    int readLen = m_serialport2.getReadBufferUsedLen();
    EXPECT_EQ(writeLen, readLen);

    char readData[1024] = {0};
    m_serialport2.readData(readData, 7);

    EXPECT_STREQ(writeData, readData);
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

    testing::GTEST_FLAG(output) = "xml:";
    testing::GTEST_FLAG(repeat) = 1;
    testing::FLAGS_gtest_filter = "CSerialPortTests.*";

    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();

    CSerialPortVirtual::deletePair(portName1);
    CSerialPortVirtual::deletePair(portName2);

#ifdef _DEBUG
    printf("\nPress Enter To Continue...\n");
    getchar();
#endif // _DEBUG

    return 0;
}