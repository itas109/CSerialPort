#include <iostream>

#ifdef _WIN32
#include <windows.h>
#define imsleep(microsecond) Sleep(microsecond) // ms
#else
#include <unistd.h>
#define imsleep(microsecond) usleep(1000 * microsecond) // ms
#endif

#include <stdio.h> // printf

#include <vector>

#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
#include "CSerialPort/IProtocolParser.h" // optional for parser
using namespace itas109;

std::string char2hexstr(const char *str, int len)
{
    static const char hexTable[17] = "0123456789ABCDEF";

    std::string result;
    for (int i = 0; i < len; ++i)
    {
        result += "0x";
        result += hexTable[(unsigned char)str[i] / 16];
        result += hexTable[(unsigned char)str[i] % 16];
        result += " ";
    }
    return result;
}

int countRead = 0;

inline unsigned short getCheckCode(const unsigned char *data, unsigned int size)
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

    unsigned short result = 0xFFFF;

    while (size--)
    {
        result = (result >> 8) ^ CRC16ModbusTable[(result ^ *data++) & 0xFF];
    }

    return result;
}

class CommonProtocolParser : public IProtocolParser
{
public:
    CommonProtocolParser() {};

    /*
    big endian
    | offset | field       | length | comment                |
    | ------ | ----------- | ------ | ---------------------- |
    | 0      | header      | 2      | 0xEB90                 |
    | 2      | version     | 1      | 0x00                   |
    | 3      | data length | 2      | N                      |
    | 5      | data        | N      |                        |
    | 5 + N  | check code  | 2      | CRC-16/MODBUS(0x18005) |
    */
    unsigned int parse(const void *buffer, unsigned int size, std::vector<IProtocolResult> &results)
    {
        const unsigned int MIN_SIZE = 7; // header(2) + version(1) + datalen(2) + crc(2)
        const unsigned int MAX_SIZE = 249;
        unsigned int offset = 0;
        unsigned int processedSize = 0;
        const unsigned char *ptr = static_cast<const unsigned char *>(buffer);

        // FSM states
        enum ParserState
        {
            STATE_IDLE,         // 空闲状态(初始状态)
            STATE_FIND_HEADER1, // 寻找帧头第一个字节0xEB
            STATE_FIND_HEADER2, // 验证帧头第二个字节0x90
            STATE_PARSE_LENGTH, // 解析数据长度
            STATE_DATA,         // 有效数据
            STATE_CHECK_CODE    // 校验
        };

        ParserState state = STATE_IDLE;
        unsigned int startOffset = 0; // 帧头起始位置
        unsigned int dataLen = 0;     // 当前帧数据长度
        unsigned int totalLen = 0;    // 当前帧总长度

        /*
        ```mermaid
        stateDiagram-v2
            [*] --> STATE_IDLE

            STATE_IDLE --> STATE_FIND_HEADER1: 剩余数据>0

            STATE_FIND_HEADER1 --> STATE_FIND_HEADER2: 找到0xEB [start=offset]
            STATE_FIND_HEADER1 --> STATE_FIND_HEADER1: 未找到0xEB [offset++]
            STATE_FIND_HEADER1 --> [*]: 数据不足 [skip=size]

            STATE_FIND_HEADER2 --> STATE_PARSE_LENGTH: 找到0x90 [offset++]
            STATE_FIND_HEADER2 --> STATE_IDLE: 未找到0x90 [offset=start+1]
            STATE_FIND_HEADER2 --> [*]: 数据不足 [skip=offset-1]

            STATE_PARSE_LENGTH --> STATE_DATA: 长度有效 [offset+=3]
            STATE_PARSE_LENGTH --> STATE_IDLE: 长度>MAX_SIZE [offset=start+1]
            STATE_PARSE_LENGTH --> [*]: 数据不足 [skip=start]

            STATE_DATA --> STATE_CHECK_CODE: 解析数据 [offset+=dataLen]
            STATE_DATA --> [*]: 数据不足 [skip=start]

            STATE_CHECK_CODE --> STATE_IDLE: 校验成功,解析下一帧 [offset=start+totalLen]
            STATE_CHECK_CODE --> STATE_IDLE: 校验失败 [offset=start+1]
            STATE_CHECK_CODE --> [*]: 数据不足 [skip=start]
        ```
        */
        while (offset < size)
        {
            switch (state)
            {
                case STATE_IDLE: // 状态 0: 空闲状态（等待数据）
                    if (offset < size)
                    {
                        state = STATE_FIND_HEADER1;
                    }
                    else
                    {
                        processedSize = offset;
                        return processedSize;
                    }
                    break;
                case STATE_FIND_HEADER1: // 状态1：寻找帧头第一个字节0xEB
                    if (ptr[offset] == 0xEB)
                    {
                        startOffset = offset;
                        state = STATE_FIND_HEADER2; // 找到0xEB，进入状态2
                    }
                    ++offset;
                    break;

                case STATE_FIND_HEADER2: // 状态2：验证帧头第二个字节0x90
                    if (offset >= size)
                    {
                        processedSize = startOffset;
                        return processedSize;
                    }
                    if (ptr[offset] == 0x90)
                    {
                        state = STATE_PARSE_LENGTH; // 找到0x90，进入状态3
                        ++offset;
                    }
                    else
                    {
                        offset = startOffset + 1; // 未找到0x90，退回状态0
                        state = STATE_IDLE;
                    }
                    break;

                case STATE_PARSE_LENGTH:   // 状态3：解析数据长度
                    if (offset + 3 > size) // 版本(1) + 数据长度(2) 需 3 字节
                    {
                        processedSize = startOffset;
                        return processedSize;
                    }
                    dataLen = (ptr[offset + 1] << 8) | ptr[offset + 2];
                    totalLen = dataLen + 7; // 总长度 = 数据长度 + 固定头尾长度

                    // 验证数据长度有效性
                    if (dataLen > MAX_SIZE)
                    {
                        offset = startOffset + 1; // 长度无效，跳过当前帧头
                        state = STATE_IDLE;
                        break;
                    }
                    offset += 3; // 移动偏移到数据区
                    state = STATE_DATA;
                    break;
                case STATE_DATA: // 状态4：有效数据(预留)
                    if (offset + dataLen > size)
                    {
                        processedSize = startOffset;
                        return processedSize;
                    }
                    offset += dataLen; // 移动偏移到校验区
                    state = STATE_CHECK_CODE;
                case STATE_CHECK_CODE: // 状态5：校验
                    if (offset + 2 > size)
                    {
                        processedSize = startOffset;
                        return processedSize;
                    }

                    const unsigned char *msgStart = ptr + startOffset;
                    unsigned short receivedCrc = (msgStart[totalLen - 2] << 8) | msgStart[totalLen - 1];
                    if (receivedCrc == getCheckCode(msgStart, totalLen - 2))
                    {
                        // check passed
#ifdef CSERIALPORT_CPP11
                        results.emplace_back(msgStart, totalLen);
#else
                        IProtocolResult result(msgStart, totalLen);
                        results.push_back(result);
#endif
                        offset = startOffset + totalLen;
                        processedSize = offset;
                        state = STATE_IDLE; // 返回状态0处理后续数据
                    }
                    else
                    {
                        offset = startOffset + 1; // skip first header byte and continue search
                        state = STATE_IDLE;
                    }
                    break;
            }
        }

        if (state == STATE_FIND_HEADER1)
        {
            processedSize = size; // not found header, skip all size
        }
        else if (state == STATE_FIND_HEADER2)
        {
            processedSize = offset - 1; // only found header1
        }

        return processedSize;
    }

    void onProtocolEvent(std::vector<IProtocolResult> &results)
    {
        for (size_t i = 0; i < results.size(); ++i)
        {
            IProtocolResult result = results.at(i);
            printf("parse result. str: %s, len: %d, hex: %s\n", (char *)result.data, result.len, char2hexstr((char *)&result.data, result.len).c_str());
        }
    }
};

int main()
{
    CSerialPort sp;
    printf("Version: %s\n\n", sp.getVersion());

    CommonProtocolParser protocolParser;
    sp.setProtocolParser(&protocolParser);

    std::vector<SerialPortInfo> m_availablePortsList = CSerialPortInfo::availablePortInfos();

    printf("AvailableFriendlyPorts:\n");

    int availablePortCount = (int)m_availablePortsList.size();

    for (int i = 1; i <= availablePortCount; ++i)
    {
        SerialPortInfo serialPortInfo = m_availablePortsList[i - 1];
        printf("%d - %s %s %s\n", i, serialPortInfo.portName, serialPortInfo.description, serialPortInfo.hardwareId);
    }

    if (m_availablePortsList.size() == 0)
    {
        printf("No valid port\n");
    }
    else
    {
        std::cout << std::endl;

        int input = -1;
        do
        {
            printf("Please Input The Index Of Port(1 - %d)\n", availablePortCount);

            std::cin >> input;

            if (input >= 1 && input <= m_availablePortsList.size())
            {
                break;
            }
        } while (true);

        const char *portName = m_availablePortsList[input - 1].portName;
        printf("Port Name: %s\n", portName);

        sp.init(portName,              // windows:COM1 Linux:/dev/ttyS0
                itas109::BaudRate9600, // baudrate
                itas109::ParityNone,   // parity
                itas109::DataBits8,    // data bit
                itas109::StopOne,      // stop bit
                itas109::FlowNone,     // flow
                4096                   // read buffer size
        );

        sp.open();
        printf("Open %s %s\n", portName, sp.isOpen() ? "Success" : "Failed");
        printf("Code: %d, Message: %s\n", sp.getLastError(), sp.getLastErrorMsg());

        // write empty frame
        unsigned char emptyFrame[7] = {0xEB, 0x90, 0x00, 0x00, 0x00, 0x17, 0x2D};
        sp.writeData(emptyFrame, sizeof(emptyFrame));

        for (;;)
        {
            imsleep(1);
        }
    }

    return 0;
}