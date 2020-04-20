#include "gtest/gtest.h"

#include "../src/SerialPort.h"
#include "../src/SerialPortInfo.h"
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

        m_availablePortsList = CSerialPortInfo::availablePorts();
        std::copy(m_availablePortsList.begin(), m_availablePortsList.end(), back_inserter(m_availablePortsVector));

        ASSERT_TRUE(m_availablePortsList.size() > 0) << "No valid port";
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
    std::list<std::string> m_availablePortsList;
    vector<std::string> m_availablePortsVector;
};

// #0_1 是否所有串口为空闲状态
TEST_F(CSerialPortTests, isAllPortIdle_0_1)
{
    std::cout << "Serial Port available Count : " << m_availablePortsVector.size() << std::endl;
    for (int i = 0; i < m_availablePortsVector.size(); i++)
    {
        m_serialport.init(m_availablePortsVector[i]);
        ASSERT_TRUE(m_serialport.open()) << m_availablePortsVector[0] << " open error, error code: " << m_serialport.getLastError();
        m_serialport.close();
    }
}

// #1_1 open port 打开未占用的串口
TEST_F(CSerialPortTests, open_1_1)
{
    m_serialport.init(m_availablePortsVector[0]);
    EXPECT_TRUE(m_serialport.open()) << m_availablePortsVector[0] << " open error, error code: " << m_serialport.getLastError();
}

// #1_2 open port 打开占用的串口
TEST_F(CSerialPortTests, open_1_2)
{
    m_serialport2.init(m_availablePortsVector[0]);
    ASSERT_TRUE(m_serialport2.open()) << m_availablePortsVector[0] << " Used error";

    m_serialport.init(m_availablePortsVector[0]);
    EXPECT_FALSE(m_serialport.open()) << m_availablePortsVector[0] << " is used,but open ok";
}

// #1_3 multiple open port 同时打开多个串口
TEST_F(CSerialPortTests, open_1_3)
{
    ASSERT_TRUE(m_availablePortsList.size() > 1) << "port count < 2";

    m_serialport.init(m_availablePortsVector[0]);
    m_serialport2.init(m_availablePortsVector[1]);

    EXPECT_TRUE(m_serialport.open()) << m_availablePortsVector[0] << " open error, error code: " << m_serialport.getLastError();
    EXPECT_TRUE(m_serialport2.open()) << m_availablePortsVector[1] << " open error, error code: " << m_serialport.getLastError();
}

// #2_1 未打开的串口,isOpened()是否与open()返回值一致
TEST_F(CSerialPortTests, isOpened_2_1)
{
    m_serialport.init(m_availablePortsVector[0]);
    bool isOpen = m_serialport.open();
    EXPECT_TRUE(isOpen == m_serialport.isOpened()) << m_availablePortsVector[0] << " function isOpened() isOpened status not equal function open() ";
}

// #2_2 被占用的串口,isOpened()是否与open()返回值一致
TEST_F(CSerialPortTests, isOpened_2_2)
{
    m_serialport2.init(m_availablePortsVector[0]);
    ASSERT_TRUE(m_serialport2.open()) << m_availablePortsVector[0] << " Used error";

    m_serialport.init(m_availablePortsVector[0]);
    bool isOpen = m_serialport.open();
    EXPECT_TRUE(isOpen == m_serialport.isOpened()) << m_availablePortsVector[0] << " function isOpened() isOpened status not equal function open() ";
}

// #3_1 open port and close 打开并关闭串口
TEST_F(CSerialPortTests, open_close_3_1)
{
    m_serialport.init(m_availablePortsVector[0]);
    EXPECT_TRUE(m_serialport.open()) << m_availablePortsVector[0] << " open error, error code: " << m_serialport.getLastError();

    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpened()) << m_availablePortsVector[0] << " close error, error code: " << m_serialport.getLastError();
}

// #3_2 open close and then open close 打开并关闭串口，再次打开并关闭串口
TEST_F(CSerialPortTests, open_close_3_2)
{
    m_serialport.init(m_availablePortsVector[0]);
    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpened()) << m_availablePortsVector[0] << " open error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpened()) << m_availablePortsVector[0] << " close error, error code: " << m_serialport.getLastError();
    
    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpened()) << m_availablePortsVector[0] << " open again error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpened()) << m_availablePortsVector[0] << " close again error, error code: " << m_serialport.getLastError();
}

// #3_3 打开并关闭串口(9600,N,1)，切换串口，再次打开串口(9600,N,1)	
TEST_F(CSerialPortTests, open_close_3_3)
{
    ASSERT_TRUE(m_availablePortsList.size() > 1) << "port count < 2";

    m_serialport.init(m_availablePortsVector[0]);
    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpened()) << m_availablePortsVector[0] << " open error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpened()) << m_availablePortsVector[0] << " close error, error code: " << m_serialport.getLastError();
    
    m_serialport.init(m_availablePortsVector[1]);
    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpened()) << m_availablePortsVector[1] << " open again error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpened()) << m_availablePortsVector[1] << " close again error, error code: " << m_serialport.getLastError();
}

// #3_4 打开并关闭串口(9600,N,1)，切换波特率，再次打开串口(115200,N,1)		
TEST_F(CSerialPortTests, open_close_3_4)
{
    m_serialport.init(m_availablePortsVector[0],9600);
    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpened()) << m_availablePortsVector[0] << " open error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpened()) << m_availablePortsVector[0] << " close error, error code: " << m_serialport.getLastError();
    
    m_serialport.init(m_availablePortsVector[0],115200);
    m_serialport.open();
    EXPECT_TRUE(m_serialport.isOpened()) << m_availablePortsVector[0] << " open again error, error code: " << m_serialport.getLastError();
    m_serialport.close();
    EXPECT_FALSE(m_serialport.isOpened()) << m_availablePortsVector[0] << " close again error, error code: " << m_serialport.getLastError();
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

#ifdef I_OS_WIN
    system("pause");
#elif defined I_OS_UNIX   // unix
    printf("Press any key to continue...");
    fgetc(stdin);
#endif
    
    return 0;
}