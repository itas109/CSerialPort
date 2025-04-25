// doctest
#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_NO_POSIX_SIGNALS // fixed SIGSTKSZ compile error on ubuntu 22
#include "doctest.h"

#include <iostream>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#define imsleep(microsecond) Sleep(microsecond) // ms
#else
#include <unistd.h>
#define imsleep(microsecond) usleep(1000 * microsecond) // ms
#endif

// ReadEventNotify
#include "CSerialPort/ithread.hpp"

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
    static ReadEventNotify &getInstance()
    {
        static ReadEventNotify instance;
        return instance;
    }

    void wait(char *portName, unsigned int &readBufferLen, unsigned int timeoutMS = 100)
    {
        if (m_cv.timeWait(m_mutex, timeoutMS, m_readBufferLen > 0))
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
			IAutoLock lock(&m_mutex);
            m_readBufferLen = readBufferLen;
            itas109::IUtils::strncpy(m_portName, portName, 256);
        }

		m_cv.notifyOne();
    }

private:
    ReadEventNotify()
	{
        memset(m_portName, 0, 256);
        m_readBufferLen = 0;
	}

private:
	IConditionVariable m_cv;
	IMutex m_mutex;

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

class CSerialPortTests
{
public:
    CSerialPortTests()  // run before test
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
        imsleep(10);
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

// #1_2 open port 打开占用的串口
TEST_CASE_FIXTURE(CSerialPortTests, "open_1_2")
{
    m_serialport2.init(portName1);
    REQUIRE_MESSAGE(m_serialport2.open(), portName1 << " open error");

    m_serialport1.init(portName1);
    CHECK_FALSE_MESSAGE(m_serialport1.open(), portName1 << " is used,but open ok");
}

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

    m_serialport1.open();
    m_serialport2.open();

    const char *writeData = "itas109";
    m_serialport1.writeData(writeData, 7);

    imsleep(10); // TODO: wait sync event

    char readData[1024] = {0};
    m_serialport2.readData(readData, 7);

    CHECK(0 == strcmp(writeData, readData));
}

// #5_1 read async 同步读
TEST_CASE_FIXTURE(CSerialPortTests, "read_async_5_1")
{
    REQUIRE_MESSAGE(m_availablePortsInfoVector.size() > 1, "port count < 2");

    MyListener listener;
    m_serialport2.connectReadEvent(&listener);

    m_serialport1.init(portName1);
    m_serialport2.init(portName2);

    m_serialport1.open();
    m_serialport2.open();

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