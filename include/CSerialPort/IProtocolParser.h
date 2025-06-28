/**
 * @file IProtocol.h
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief common comunication protocol implement 通用通信协议接口类
 */
#ifndef __I_PROTOCOL_PARSER_H__
#define __I_PROTOCOL_PARSER_H__

#include <string.h> // memcpy memset
#include <vector>

namespace itas109
{
/**
 * @brief protocol result implement 协议解析结果接口
 *
 */
struct IProtocolResult
{
    unsigned char data[256];
    unsigned int len;

    IProtocolResult()
        : len(0)
    {
        memset(data, 0, 256);
    }

    IProtocolResult(const void *_data, unsigned int _len)
        : len(_len > 256 ? 256 : _len)
    {
        memset(data, 0, 256);
        memcpy(data, _data, len);
    }
};

class IProtocolParser
{
public:
    IProtocolParser() {};
    virtual ~IProtocolParser() {};

    /**
     * @brief parse frame 解析帧
     *
     * @param buffer [in] buffer 缓冲数据
     * @param size [in] data length 数据长度
     * @param results [out] 帧数据
     * @return return processedSize 已完成解析的数据大小(ringbuffer需要跳过的数据大小)
     */
    virtual unsigned int parse(const void *buffer, unsigned int size, std::vector<IProtocolResult> &results) = 0;

    virtual void onProtocolEvent(std::vector<IProtocolResult> &results) = 0;
};

/**
 * @brief length field based protocol parser 基于长度字段的协议解析器
 * 
 * +--------+-----+--------+------+------+--------+
 * | header | ... | length | data |check | footer |
 * +--------+-----+--------+------+------+--------+
 *
 */
class LengthFieldBasedProtocolParser : public IProtocolParser
{
public:
    LengthFieldBasedProtocolParser(
        unsigned int headerFieldLength, // 0 1 2
        unsigned char header[2],
        unsigned int lengthFieldLength, // > 0
        unsigned int lengthFieldOffset,
        unsigned int lengthAdjustment,
        unsigned int lengthFieldMaxValue,
        unsigned int checkCodeFieldLength,
        unsigned int footerFieldLength,    // 0 1 2
        unsigned char footer[2],
        bool isBigEndian = true)
        : m_headerFieldLength(headerFieldLength)
        , m_lengthFieldLength(lengthFieldLength)
        , m_lengthFieldOffset(lengthFieldOffset)
        , m_lengthAdjustment(lengthAdjustment)
        , m_lengthFieldMaxValue(lengthFieldMaxValue)
        , m_checkCodeFieldLength(checkCodeFieldLength)
        , m_footerFieldLength(footerFieldLength)
        , m_isBigEndian(isBigEndian)
    {
        memcpy(m_header, header, 2);
        memcpy(m_footer, footer, 2);
    };

    virtual ~LengthFieldBasedProtocolParser() {}

    virtual bool isCheckCodeValid(const unsigned char* data, unsigned int size)
    {
        return true;
    }

    unsigned int parse(const void *buffer, unsigned int size, std::vector<IProtocolResult> &results)
    {
        if (m_headerFieldLength > 2 || 0 == m_lengthFieldLength || m_lengthFieldOffset < m_headerFieldLength || m_footerFieldLength > 2)
        {
            return 0;
        }

        unsigned int offset = 0;
        unsigned int processedSize = 0;
        const unsigned char *ptr = static_cast<const unsigned char *>(buffer);

        // FSM states
        // STATE_IDLE -> STATE_HEADER  -> STATE_PARSE_LENGTH  -> STATE_DATA -> STATE_CHECK_CODE -> STATE_FOOTER -> STATE_IDLE
        enum ParserState
        {
            STATE_IDLE,         // 空闲状态
            STATE_HEADER,       // 帧头
            STATE_PARSE_LENGTH, // 解析数据长度
            STATE_DATA,         // 有效数据
            STATE_CHECK_CODE,   // 校验
            STATE_FOOTER        // 帧尾
        };

        ParserState state = STATE_IDLE;
        unsigned int startOffset = 0; // 帧头起始位置
        unsigned int dataLen = 0;     // 当前帧数据长度
        unsigned int totalLen = 0;    // 当前帧总长度

        while (offset <= size)
        {
            switch (state)
            {
                case STATE_IDLE: // 状态 0: 空闲状态（等待数据）
                    if (offset < size)
                    {
                        state = STATE_HEADER;
                    }
                    else
                    {
                        processedSize = offset;
                        return processedSize;
                    }
                    break;
                case STATE_HEADER: // 状态1：帧头
                    if (0 == m_headerFieldLength)
                    {
                        startOffset = offset;
                        state = STATE_PARSE_LENGTH;
                    }
                    else if (1 == m_headerFieldLength)
                    {
                        if (ptr[offset] == m_header[0])
                        {
                            startOffset = offset;
                            state = STATE_PARSE_LENGTH;
                            // ++offset;
                        }
                        ++offset;
                    }
                    else if (2 == m_headerFieldLength)
                    {
                        if (offset + 1 >= size)
                        {
                            processedSize = offset;
                            return processedSize;
                        }
                        if (ptr[offset] == m_header[0] && ptr[offset + 1] == m_header[1])
                        {
                            startOffset = offset;
                            state = STATE_PARSE_LENGTH;
                            offset += 2;
                        }
                        else
                        {
                            ++offset;
                        }
                    }
                    break;
                case STATE_PARSE_LENGTH: // 状态2：解析数据长度
                    if (offset + m_lengthFieldLength > size)
                    {
                        processedSize = startOffset;
                        return processedSize;
                    }

                    if (m_isBigEndian)
                    {
                        dataLen = 0;
                        for (size_t i = 0; i < m_lengthFieldLength; ++i)
                        {
                            dataLen = (dataLen << 8) | ptr[startOffset + m_lengthFieldOffset + i];
                        }
                    }
                    else
                    {
                        dataLen = 0;
                        for (size_t i = 0; i < m_lengthFieldLength; ++i)
                        {
                            dataLen |= (static_cast<unsigned int>(ptr[startOffset + m_lengthFieldOffset + i]) << (i * 8));
                        }
                    }

                    totalLen = m_lengthFieldOffset + m_lengthFieldLength + dataLen + m_lengthAdjustment; // 总长度 = 长度字段偏移 + 长度字段大小+ 解析长度 + 调节长度(类型、版本、校验位、帧尾等)

                    // 验证数据长度有效性
                    if (dataLen > m_lengthFieldMaxValue)
                    {
                        offset = startOffset + 1; // 长度无效，跳过当前帧头
                        state = STATE_IDLE;
                        continue;
                    }
                    offset = startOffset + m_lengthFieldOffset + m_lengthFieldLength; // 移动偏移到数据区
                    state = STATE_DATA;
                    break;
                case STATE_DATA: // 状态3：有效数据(预留)
                    if (offset + dataLen > size)
                    {
                        processedSize = startOffset;
                        return processedSize;
                    }
                    offset += dataLen; // 移动偏移到校验区
                    state = STATE_CHECK_CODE;
                    break;
                case STATE_CHECK_CODE: // 状态4：校验
                    if (offset + m_checkCodeFieldLength > size)
                    {
                        processedSize = startOffset;
                        return processedSize;
                    }

                    if (0 == m_checkCodeFieldLength)
                    {
                        state = STATE_FOOTER;
                    }
                    else
                    {
                        // check code
                        if (isCheckCodeValid(&ptr[startOffset], totalLen - m_footerFieldLength))
                        {
                            offset += m_checkCodeFieldLength; // 移动偏移到帧尾
                            state = STATE_FOOTER;
                        }
                        else
                        {
                            offset = startOffset + 1; // skip first header byte and continue search
                            state = STATE_IDLE;
                            continue;
                        }
                    }
                    break;
                case STATE_FOOTER: // 状态5：帧尾
                    if (offset + m_footerFieldLength > size)
                    {
                        processedSize = startOffset;
                        return processedSize;
                    }

                    if (0 == m_footerFieldLength)
                    {
                        // check passed
#ifdef CSERIALPORT_CPP11
                        results.emplace_back(&ptr[startOffset], totalLen);
#else
                        IProtocolResult result(&ptr[startOffset], totalLen);
                        results.push_back(result);
#endif
                        offset = startOffset + totalLen;
                        processedSize = offset;
                        state = STATE_IDLE; // 返回状态0处理后续数据
                    }
                    else if (1 == m_footerFieldLength && ptr[offset] == m_footer[0])
                    {
                        // check passed
#ifdef CSERIALPORT_CPP11
                        results.emplace_back(&ptr[startOffset], totalLen);
#else
                        IProtocolResult result(&ptr[startOffset], totalLen);
                        results.push_back(result);
#endif
                        offset = startOffset + totalLen;
                        processedSize = offset;
                        state = STATE_IDLE; // 返回状态0处理后续数据
                    }
                    else if (2 == m_footerFieldLength && ptr[offset] == m_footer[0] && ptr[offset + 1] == m_footer[1])
                    {
                        // check passed
#ifdef CSERIALPORT_CPP11
                        results.emplace_back(&ptr[startOffset], totalLen);
#else
                        IProtocolResult result(&ptr[startOffset], totalLen);
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

        return processedSize;
    }

    virtual void onProtocolEvent(std::vector<IProtocolResult> &results) = 0;

private:
    unsigned int m_headerFieldLength; // 0 1 2
    unsigned char m_header[2];
    unsigned int m_lengthFieldOffset;
    unsigned int m_lengthFieldLength; // > 0
    unsigned int m_lengthAdjustment;
    unsigned int m_lengthFieldMaxValue;
    unsigned int m_checkCodeFieldLength; // 0 1 2
    unsigned int m_footerFieldLength;    // 0 1 2
    unsigned char m_footer[2];
    bool m_isBigEndian;
};

/**
 * @brief delimiter based protocol parser 基于分隔符的协议解析器
 * 
 *  +--------+------+-------+--------+
 *  | header | data | check | footer |
 *  +--------+------+-------+--------+ 
 *
 * AT command: AT...<CR><LF>
 * unsigned char header[2] = {'A', 'T'} // AT 
 * unsigned char footer[2] = {'\r', '\n'}; // \r\n
 * DelimiterBasedProtocolParser parser(2, header, 2, footer);
 * 
 * NMEA-0183: $aaccc,ddd,ddd,…,ddd*hh<CR><LF>
 * unsigned char header[2] = {'$', 0x00} // $
 * unsigned char footer[2] = {'\r', '\n'}; // \r\n
 * isCheckCodeValid => XOR('$','*')
 * DelimiterBasedProtocolParser parser(1, header, 2, footer);
 */
class DelimiterBasedProtocolParser : public IProtocolParser
{
public:
    DelimiterBasedProtocolParser(
        unsigned int headerFieldLength, // 0 1 2
        unsigned char header[2],
        unsigned int footerFieldLength, // 1 2
        unsigned char footer[2])
        : m_headerFieldLength(headerFieldLength)
        , m_footerFieldLength(footerFieldLength)
    {
        memcpy(m_header, header, 2);
        memcpy(m_footer, footer, 2);
    };

    virtual ~DelimiterBasedProtocolParser() {}

    virtual bool isCheckCodeValid(const unsigned char *data, unsigned int size)
    {
        return true;
    }

    unsigned int parse(const void *buffer, unsigned int size, std::vector<IProtocolResult> &results)
    {
        if (m_headerFieldLength > 2 || m_footerFieldLength > 2 || m_footerFieldLength < 1)
        {
            return 0;
        }

        unsigned int offset = 0;
        unsigned int processedSize = 0;
        const unsigned char *ptr = static_cast<const unsigned char *>(buffer);

        // FSM states
        // STATE_IDLE -> STATE_HEADER -> STATE_FOOTER -> STATE_CHECK_CODE -> STATE_IDLE
        enum ParserState
        {
            STATE_IDLE,      // 空闲状态
            STATE_HEADER,    // 帧头
            STATE_FOOTER,    // 帧尾
            STATE_CHECK_CODE // 校验
        };

        ParserState state = STATE_IDLE;
        unsigned int startOffset = 0; // 帧头起始位置

        while (offset <= size)
        {
            switch (state)
            {
                case STATE_IDLE: // 状态 0: 空闲状态
                    if (offset < size)
                    {
                        state = STATE_HEADER;
                    }
                    else
                    {
                        processedSize = offset;
                        return processedSize;
                    }
                    break;
                case STATE_HEADER: // 状态1：帧头
                    if (0 == m_headerFieldLength)
                    {
                        startOffset = offset;
                        state = STATE_FOOTER;
                        continue;
                    }
                    else if (1 == m_headerFieldLength)
                    {
                        if (ptr[offset] == m_header[0])
                        {
                            startOffset = offset;
                            state = STATE_FOOTER;
                            // ++offset;
                        }
                        ++offset;
                    }
                    else if (2 == m_headerFieldLength)
                    {
                        if (offset + 1 >= size)
                        {
                            processedSize = offset;
                            return processedSize;
                        }
                        if (ptr[offset] == m_header[0] && ptr[offset + 1] == m_header[1])
                        {
                            startOffset = offset;
                            state = STATE_FOOTER;
                            offset += 2;
                        }
                        else
                        {
                            ++offset;
                        }
                    }
                    break;
                case STATE_FOOTER: // 状态2：帧尾
                    if (offset + m_footerFieldLength > size)
                    {
                        processedSize = startOffset;
                        return processedSize;
                    }

                    if (1 == m_footerFieldLength && ptr[offset] == m_footer[0])
                    {
                        offset += 1;
                        state = STATE_CHECK_CODE;
                    }
                    else if (2 == m_footerFieldLength && ptr[offset] == m_footer[0] && ptr[offset + 1] == m_footer[1])
                    {
                        offset += 2;
                        state = STATE_CHECK_CODE;
                    }
                    else
                    {
                        ++offset;
                    }
                    break;
                case STATE_CHECK_CODE: // 状态3: 校验
                    if (isCheckCodeValid(&ptr[startOffset], offset - startOffset))
                    {
#ifdef CSERIALPORT_CPP11
                        results.emplace_back(&ptr[startOffset], offset - startOffset);
#else
                        IProtocolResult result(&ptr[startOffset], offset - startOffset);
                        results.push_back(result);
#endif
                        processedSize = offset;
                        state = STATE_IDLE;
                    }
                    else
                    {
                        offset = startOffset + 1; // skip first header byte and continue search
                        state = STATE_IDLE;
                    }
                    break;
            }
        }

        return processedSize;
    }

    virtual void onProtocolEvent(std::vector<IProtocolResult> &results) = 0;

private:
    unsigned int m_headerFieldLength; // 0 1 2
    unsigned char m_header[2];
    unsigned int m_footerFieldLength; // 1 2
    unsigned char m_footer[2];
};

} // namespace itas109

#endif // __I_PROTOCOL_PARSER_H__