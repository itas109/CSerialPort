/**
 * @file iutils.hpp
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief lightweight cross-platform utils library 轻量级跨平台工具类
 */
#ifndef __I_UTILS_HPP__
#define __I_UTILS_HPP__

namespace itas109
{
/**
 * @brief utils class 工具类
 *
 * @tparam T
 */
class IUtils
{
public:
    IUtils(){};
    ~IUtils(){};

    static char *strncpy(char *dest, const char *src, unsigned int count)
    {
        // assert(dest != NULL && src != NULL && count != 0);

        char *cp = dest;

        while (--count && (*cp++ = *src++) != '\0')
        {
        }

        if (0 == count)
        {
            *cp = '\0';
        }
        return (dest);
    }

    static char *strncat(char *dest, const char *src, unsigned int count)
    {
        // assert(dest != NULL && src != NULL);

        char *cp = dest;

        while (*cp) // find dest end
        {
            cp++;
        }

        while (--count && (*cp++ = *src++) != '\0')
        {
        }

        if (0 == count)
        {
            *cp = '\0';
        }
        return (dest);
    }

    static char *charToHexStr(char *dest, const char *src, unsigned int count)
    {
        // assert(dest != NULL && src != NULL);

        static const char hexTable[17] = "0123456789ABCDEF";

        for (unsigned int i = 0; i < count; ++i)
        {
            dest[i * 2] = hexTable[(unsigned char)src[i] / 16];
            dest[i * 2 + 1] = hexTable[(unsigned char)src[i] % 16];
        }
        dest[count * 2] = '\0';

        return (dest);
    }
};
} // namespace itas109
#endif // __I_UTILS_HPP__
