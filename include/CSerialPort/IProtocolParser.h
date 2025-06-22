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
        memset(&data, 0, 256);
    }

    IProtocolResult(const unsigned char *_data, unsigned int _len)
        : len(_len > 256 ? 256 : _len)
    {
        memcpy(&data, _data, len);
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
     * @param skipSize [out] 需要跳过的数据大小，即已完成解析的数据大小
     * @param frames [out] 帧数据
     * @return return number Of bytes read 返回读取字节数
     * @retval -1 read error 读取错误
     * @retval [other] return number Of bytes read 返回读取字节数
     */
    virtual void parse(const void *buffer, unsigned int size, unsigned int &skipSize, std::vector<IProtocolResult> &results) = 0;

    virtual void onProtocolEvent(std::vector<IProtocolResult> &results) = 0;
};
} // namespace itas109

#endif // __I_PROTOCOL_PARSER_H__