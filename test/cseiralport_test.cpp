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

#include "gtest/gtest.h"

#include "CSerialPort/iutils.hpp"
#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
#include "CSerialPortVirtual.h"
using namespace itas109;

static char portName1[256];
static char portName2[256];

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

            // 防止太快导致串口没有被释放
            imsleep(100);
        }

        if (m_serialport2.isOpen())
        {
            m_serialport2.close();

            // 防止太快导致串口没有被释放
            imsleep(100);
        }
    }
    CSerialPort m_serialport;
    CSerialPort m_serialport2;
    std::vector<SerialPortInfo> m_availablePortsInfoVector;
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

int main(int argc, char **argv)
{
    int iRet = 0;

#ifdef _WIN32
    itas109::IUtils::strncpy(portName1, "COM40", 256);
    itas109::IUtils::strncpy(portName2, "COM50", 256);
    bool isOk = CSerialPortVirtual::createPair(portName1, portName2);
    printf("create virtual pair %s - %s %s\n", portName1, portName2, isOk ? "success" : "failed");
#else
    itas109::IUtils::strncpy(portName1, "", 256);
    itas109::IUtils::strncpy(portName2, "", 256);
    std::thread t(&CSerialPortVirtual::createPair, portName1, portName2);
    t.detach();
#endif

    try
    {
        testing::InitGoogleTest(&argc, argv);
        iRet = RUN_ALL_TESTS();
    }
    catch (std::exception &e)
    {
        std::cerr << "Unhandled Exception: " << e.what() << std::endl;
    }

    CSerialPortVirtual::deletePair(portName1);
    CSerialPortVirtual::deletePair(portName2);

    // printf("Press Enter To Continue...");
    // getchar();

    return 0;
}