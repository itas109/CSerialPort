#include "gtest/gtest.h"

#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
using namespace itas109;

#include <iostream>
#include <vector>
using namespace std;

#ifdef I_OS_WIN
    #define imsleep(x) Sleep(x)
#elif defined I_OS_UNIX   // unix
    #define imsleep(x) usleep(1000*x)
#else
    #define imsleep(x)
#endif

namespace
{

class CSerialPortTests : public testing::Test
{
protected:
    virtual void SetUp() //run before test
    {
        // std::cout << "CSerialPort Version : " << m_serialport.getVersion() << std::endl;

        m_availablePortsInfoVector = CSerialPortInfo::availablePortInfos();

        ASSERT_TRUE(m_availablePortsInfoVector.size() > 0) << "No valid port";
    }

    virtual void TearDown() //run after test
    {
        if(m_serialport.isOpened())
        {
            m_serialport.close();

            // 防止太快导致串口没有被释放
            imsleep(100);
        }

        if(m_serialport2.isOpened())
        {
            m_serialport2.close();

            // 防止太快导致串口没有被释放
            imsleep(100);
        }
    }

    CSerialPort m_serialport;
    CSerialPort m_serialport2;
    vector<SerialPortInfo> m_availablePortsInfoVector;
};

// #0_1 是否所有串口为空闲状态
TEST_F(CSerialPortTests, isAllPortIdle_0_1)
{
    std::cout << "Serial Port available Count : " << m_availablePortsInfoVector.size() << std::endl;
    for (int i = 0; i < m_availablePortsInfoVector.size(); i++)
    {
        m_serialport.init(m_availablePortsInfoVector[i].portName);
        ASSERT_TRUE(m_serialport.open()) << m_availablePortsInfoVector[0].portName << " open error, error code: " << m_serialport.getLastError();
        m_serialport.close();
    }
}

// #1_1 open port 打开未占用的串口
TEST_F(CSerialPortTests, open_1_1)
{
    m_serialport.init(m_availablePortsInfoVector[0].portName);
    EXPECT_TRUE(m_serialport.open()) << m_availablePortsInfoVector[0].portName << " open error, error code: " << m_serialport.getLastError();
}

// #1_2 open port 打开占用的串口
TEST_F(CSerialPortTests, open_1_2)
{
    m_serialport2.init(m_availablePortsInfoVector[0].portName);
    ASSERT_TRUE(m_serialport2.open()) << m_availablePortsInfoVector[0].portName << " Used error";

    m_serialport.init(m_availablePortsInfoVector[0].portName);
    EXPECT_FALSE(m_serialport.open()) << m_availablePortsInfoVector[0].portName << " is used,but open ok";
}

// #1_3 multiple open port 同时打开多个串口
TEST_F(CSerialPortTests, open_1_3)
{
    ASSERT_TRUE(m_availablePortsInfoVector.size() > 1) << "port count < 2";

    m_serialport.init(m_availablePortsInfoVector[0].portName);
    m_serialport2.init(m_availablePortsInfoVector[1].portName);

    EXPECT_TRUE(m_serialport.open()) << m_availablePortsInfoVector[0].portName << " open error, error code: " << m_serialport.getLastError();
    EXPECT_TRUE(m_serialport2.open()) << m_availablePortsInfoVector[1].portName << " open error, error code: " << m_serialport.getLastError();
}

// #2_1 未打开的串口,isOpened()是否与open()返回值一致
TEST_F(CSerialPortTests, isOpened_2_1)
{
    m_serialport.init(m_availablePortsInfoVector[0].portName);
    bool isOpen = m_serialport.open();
    EXPECT_TRUE(isOpen == m_serialport.isOpened()) << m_availablePortsInfoVector[0].portName << " function isOpened() isOpened status not equal function open() ";
}

// #2_2 被占用的串口,isOpened()是否与open()返回值一致
TEST_F(CSerialPortTests, isOpened_2_2)
{
    m_serialport2.init(m_availablePortsInfoVector[0].portName);
    ASSERT_TRUE(m_serialport2.open()) << m_availablePortsInfoVector[0].portName << " Used error";

    m_serialport.init(m_availablePortsInfoVector[0].portName);
    bool isOpen = m_serialport.open();
    EXPECT_TRUE(isOpen == m_serialport.isOpened()) << m_availablePortsInfoVector[0].portName << " function isOpened() isOpened status not equal function open() ";
}

// #3_1 open port and close 打开并关闭串口
TEST_F(CSerialPortTests, open_close_3_1)
{
    m_serialport.init(m_availablePortsInfoVector[0].portName);
    EXPECT_TRUE(m_serialport.open()) << m_availablePortsInfoVector[0].portName << " open error, error code: " << m_serialport.getLastError();

    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpened()) << m_availablePortsInfoVector[0].portName << " close error, error code: " << m_serialport.getLastError();
}

// #3_2 open close and then open close 打开并关闭串口，再次打开并关闭串口
TEST_F(CSerialPortTests, open_close_3_2)
{
    m_serialport.init(m_availablePortsInfoVector[0].portName);
    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpened()) << m_availablePortsInfoVector[0].portName << " open error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpened()) << m_availablePortsInfoVector[0].portName << " close error, error code: " << m_serialport.getLastError();
    
    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpened()) << m_availablePortsInfoVector[0].portName << " open again error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpened()) << m_availablePortsInfoVector[0].portName << " close again error, error code: " << m_serialport.getLastError();
}

// #3_3 打开并关闭串口(9600,N,1)，切换串口，再次打开串口(9600,N,1)	
TEST_F(CSerialPortTests, open_close_3_3)
{
    ASSERT_TRUE(m_availablePortsInfoVector.size() > 1) << "port count < 2";

    m_serialport.init(m_availablePortsInfoVector[0].portName);
    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpened()) << m_availablePortsInfoVector[0].portName << " open error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpened()) << m_availablePortsInfoVector[0].portName << " close error, error code: " << m_serialport.getLastError();
    
    m_serialport.init(m_availablePortsInfoVector[1].portName);
    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpened()) << m_availablePortsInfoVector[1].portName << " open again error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpened()) << m_availablePortsInfoVector[1].portName << " close again error, error code: " << m_serialport.getLastError();
}

// #3_4 打开并关闭串口(9600,N,1)，切换波特率，再次打开串口(115200,N,1)		
TEST_F(CSerialPortTests, open_close_3_4)
{
    m_serialport.init(m_availablePortsInfoVector[0].portName,9600);
    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpened()) << m_availablePortsInfoVector[0].portName << " open error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpened()) << m_availablePortsInfoVector[0].portName << " close error, error code: " << m_serialport.getLastError();
    
    m_serialport.init(m_availablePortsInfoVector[0].portName,115200);
    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpened()) << m_availablePortsInfoVector[0].portName << " open again error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpened()) << m_availablePortsInfoVector[0].portName << " close again error, error code: " << m_serialport.getLastError();
}

} // namespace

int main(int argc, char **argv)
{
    int iRet = 0;
    try
    {
        testing::InitGoogleTest(&argc, argv);
        iRet = RUN_ALL_TESTS();
    }
    catch (std::exception &e)
    {
        std::cerr << "Unhandled Exception: " << e.what() << std::endl;
    }

// #ifdef I_OS_WIN
//    system("pause");
// #elif defined I_OS_UNIX   // unix
//    printf("Press any key to continue...");
//    fgetc(stdin);
// #endif
    
    return 0;
}