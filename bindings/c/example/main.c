/**
 * @file main.c
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief C CSerialPort Example  C的CSerialPort示例程序
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> // malloc

#include "cserialport.h"

int countRead = 0;

void char2hexstr(char *dest, const char *src, int len)
{
    static const char hexTable[17] = "0123456789ABCDEF";

    int i;
    for (i = 0; i < len; ++i)
    {
        // 0x + two bit hex +  one bit space => 5 bit
        dest[5 * i + 0] = '0';
        dest[5 * i + 1] = 'x';
        dest[5 * i + 2] = hexTable[(unsigned char)src[i] / 16];
        dest[5 * i + 3] = hexTable[(unsigned char)src[i] % 16];
        dest[5 * i + 4] = ' ';
    }
    dest[5 * len] = '\0';
}

void onReadEvent(i_handle_t handle, const char *portName, unsigned int readBufferLen)
{
    if (readBufferLen > 0)
    {
        char *data = malloc(readBufferLen + 1); // '\0'

        if (data)
        {
            // read
            int recLen = CSerialPortReadData(handle, data, readBufferLen);

            if (recLen > 0)
            {
                data[recLen] = '\0';

                char *hexStr = malloc(5 * recLen + 1); // 0xAB

                char2hexstr(hexStr, data, recLen);
                printf("%s - Count: %d Length: %d, Str: %s, Hex: %s\n", portName, ++countRead, recLen, data, hexStr);

                if (hexStr)
                {
                    free(hexStr);
                    hexStr = NULL;
                }

                // return receive data
                CSerialPortWriteData(handle, data, recLen);
            }

            free(data);
            data = NULL;
        }
    }
}

void onHotPlugEvent(i_handle_t handle, const char *portName, int isAdd)
{
    printf("portName: %s, isAdded: %d\n", portName, isAdd);
}

unsigned short getCheckCode(const unsigned char *data, unsigned int size)
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

unsigned int parser(i_handle_t handle, const void *buffer, unsigned int size, struct ProtocolResultArray *results)
{
    const unsigned int MIN_SIZE = 7; // header(2) + version(1) + datalen(2) + crc(2)
    const unsigned int MAX_SIZE = 249;
    unsigned int offset = 0;
    unsigned int processedSize = 0;
    const unsigned char *ptr = (const unsigned char *)buffer;

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

    enum ParserState state = STATE_IDLE;
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
                    if (0 == results->size)
                    {
                        results->result = (struct ProtocolResult *)malloc(sizeof(struct ProtocolResult));
                        memcpy(results->result->data, msgStart, totalLen);
                        results->result->len = totalLen;
                        results->size = 1;
                    }
                    else
                    {
                        unsigned int newSize = results->size + 1;
                        struct ProtocolResult *newResult = (struct ProtocolResult *)realloc(results->result, newSize * sizeof(struct ProtocolResult));
                        if (NULL == newResult)
                        {
                            return 0; // alloc failed
                        }
                        newResult[results->size].len = totalLen;
                        memcpy(newResult[results->size].data, msgStart, totalLen);
                        results->result = newResult;
                        results->size = newSize;
                    }

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

void onProtocolEvent(i_handle_t handle, struct ProtocolResult *result)
{
    if (result)
    {
        char hexStr[201];
        char2hexstr(hexStr, result->data, result->len > 100 ? 100 : result->len);
        printf("parse result. str: %s, len: %d, hex(100): %s\n", result->data, result->len, hexStr);
        free(result);
    }
}

int main()
{
    i_handle_t handle = 0;
    handle = CSerialPortMalloc();
    printf("Version: %s\n\n", CSerialPortGetVersion(handle));

    printf("Available Friendly Ports:\n");

    struct SerialPortInfoArray portInfoArray = {0};
    CSerialPortAvailablePortInfosMalloc(&portInfoArray);

    // connect for read
    CSerialPortConnectReadEvent(handle, onReadEvent);
    // connect for hot plug
    CSerialPortConnectHotPlugEvent(handle, onHotPlugEvent);

    unsigned int i;
    for (i = 0; i < portInfoArray.size; ++i)
    {
        printf("%u - %s %s\n", i + 1, portInfoArray.portInfo[i].portName, portInfoArray.portInfo[i].description);
    }

    if (portInfoArray.size == 0)
    {
        printf("No Valid Port\n");
    }
    else
    {
        printf("\n");

        unsigned int input = 0;
        do
        {
            printf("Please Input The Index Of Port(1 - %d)\n", portInfoArray.size);

            (void)scanf("%u", &input);

            if (input >= 1 && input <= portInfoArray.size)
            {
                break;
            }
        } while (1);

        char inputParseProtocol;
        printf("Is Parse Protocol?(y/n)\n");
        (void)scanf(" %c", &inputParseProtocol);
        if ('y' == inputParseProtocol || 'Y' == inputParseProtocol)
        {
            // set protocol parser
            CSerialPortSetProtocolParser(handle, parser, onProtocolEvent);
            printf("Parse Protocol: y\n\n 0      1      2      3      4      5      6...    N-2    N-1\n+------+------+------+------+------+------+-----+------+------+\n| 0xEB | 0x90 | Ver  | LenH | LenL | Data | ... | CRCH | CRCL |\n+------+------+------+------+------+------+-----+------+------+\n|<---------- fixed header -------->|<-- data -->|<--- CRC --->|\nexample: EB90 00 0007 20211025000000 7F52\n\n");
        }
        else
        {
            printf("Parse Protocol: n\n");
        }

        char portName[256] = {0};
        strcpy(portName, portInfoArray.portInfo[input - 1].portName);
        printf("Port Name: %s\n", portName);

        CSerialPortAvailablePortInfosFree(&portInfoArray);

        CSerialPortInit(handle,
                        portName,   // windows:COM1 Linux:/dev/ttyS0
                        9600,       // baudrate
                        ParityNone, // parity
                        DataBits8,  // data bit
                        StopOne,    // stop bit
                        FlowNone,   // flow
                        4096        // read buffer size
        );
        CSerialPortSetReadIntervalTimeout(handle, 0); // read interval timeout

        CSerialPortOpen(handle);

        printf("Open %s %s\n", portName, 1 == CSerialPortIsOpen(handle) ? "Success" : "Failed");
        printf("Code: %d, Message: %s\n", CSerialPortGetLastError(handle), CSerialPortGetLastErrorMsg(handle));

        // write hex data
        char hex[5];
        hex[0] = 0x31;
        hex[1] = 0x32;
        hex[2] = 0x33;
        hex[3] = 0x34;
        hex[4] = 0x35;
        CSerialPortWriteData(handle, hex, sizeof(hex));

        // write str data
        CSerialPortWriteData(handle, "itas109", 7);
    }

    for (;;)
    {
    }

    CSerialPortDisconnectReadEvent(handle);

    CSerialPortFree(handle);

    return 0;
}