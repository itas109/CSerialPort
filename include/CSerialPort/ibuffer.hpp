/**
 * @file ibuffer.hpp
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief lightweight cross-platform buffer library 轻量级跨平台缓冲区类
 */
#ifndef __I_BUFFER_HPP__
#define __I_BUFFER_HPP__

#include <memory>  // 添加std::unique_ptr支持
#include <cstring> // 添加memcpy支持(C++11)

static unsigned int nextPowerOf2(unsigned int num)
{
    // 2^30 = 1 << 30 = 1073741824
    // 2^31 = 1 << 31 = 2147483648
    // unsigned int(Max PowerOf2) 2^31 = 1 << 31 = 2147483648
    // unsigned int(Max) 2^32 -1 = 4294967295

    // because MSB(mirroring significant bit), so 2^30 not 2^31
    // m_maxBufferSize = 1073741824;// 2^30
    // m_maxMirrorBufferIndex = 2 * m_maxBufferSize - 1 = 2147483647
    if (num == 0 || num > 1073741824 /* 2^30 */)
    {
        return 4096;
    }

    --num;
    num |= num >> 1;
    num |= num >> 2;
    num |= num >> 4;
    num |= num >> 8;
    num |= num >> 16;

    return ++num;
}

namespace itas109
{
/**
 * @brief Buffer base class 缓冲区基类
 *
 * @tparam T
 */
template <typename T>
class Buffer
{
public:
    Buffer() = default;
    virtual ~Buffer() = default;

    virtual int write(const T *data, unsigned int size) = 0;
    virtual int read(T *data, unsigned int size) = 0;
    virtual int peek(T *data, unsigned int size) = 0;
    virtual int skip(unsigned int skipSize) = 0;
    virtual bool isFull() const = 0;
    virtual bool isEmpty() const = 0;
    virtual unsigned int getUsedLen() const = 0;
    virtual unsigned int getUnusedLen() const = 0;
    virtual unsigned int getBufferSize() const = 0;
};

/**
 * @brief RingBuffer MSB(mirroring significant bit) 环形缓冲区(镜像指示位方式)
 *
 * @tparam T
 */
template <typename T>
class RingBuffer : public Buffer<T>
{
public:
    /**
     * @brief Construct a new Ring Buffer object 构造函数
     *
     */
    RingBuffer()
        : m_readMirrorIndex{0}
        , m_writeMirrotIndex{0}
        , m_maxBufferSize{4096} ///< must power of two
        , m_mask{m_maxBufferSize - 1}
        , m_maxMirrorBufferIndex{2 * m_maxBufferSize - 1}
        , m_buffer{new T[m_maxBufferSize]}
    {
    }

    /**
     * @brief Construct a new Ring Buffer object 通过缓冲区大小构造
     *
     * @param maxBufferSize [in] buffer size 缓冲区大小
     */
    RingBuffer(unsigned int maxBufferSize)
        : m_readMirrorIndex{0}
        , m_writeMirrotIndex{0}
        , m_maxBufferSize{(maxBufferSize && (0 == (maxBufferSize & (maxBufferSize - 1)))) ? maxBufferSize : nextPowerOf2(maxBufferSize)} ///< must power of two
        , m_mask{m_maxBufferSize - 1}
        , m_maxMirrorBufferIndex{2 * m_maxBufferSize - 1}
        , m_buffer{new T[m_maxBufferSize]}
    {
    }

    /**
     * @brief Destroy the Ring Buffer object 析构函数
     *
     */
    ~RingBuffer() override = default;

    RingBuffer(const RingBuffer &) = delete;
    RingBuffer &operator=(const RingBuffer &) = delete;

    /**
     * @brief write data to buffer 向缓冲区写数据
     *
     * @param data [in] write data 待写入数据
     * @param size [in] write data size 待写入大小
     * @return return write data size 返回写入数据大小
     */
    int write(const T *data, unsigned int size) override
    {
        for (unsigned int i = 0; i < size; ++i)
        {
            m_buffer[m_writeMirrotIndex & m_mask] = data[i];
            if (isFull())
            {
                m_readMirrorIndex = (m_readMirrorIndex + 1) & m_maxMirrorBufferIndex; ///< head and tail pointers incrementation is done modulo 2*size
            }
            m_writeMirrotIndex = (m_writeMirrotIndex + 1) & m_maxMirrorBufferIndex;
        }

        return size;
    }

    /**
     * @brief read data from buffer 从缓冲区读数据
     *
     * @param data [out] read data to save 待读取数据存储
     * @param size [in] read data size 待读取数据大小
     * @return return read data size 返回读取数据大小
     * @retval 0 buffer empty 缓冲区空
     * @retval [other] return read data size 返回读取数据大小
     */
    int read(T *data, unsigned int size) override
    {
        if (isEmpty() || 0 == size)
        {
            return 0;
        }

        // check and set size to [1, n]
        const unsigned int usedLen = getUsedLen();
        if (size > usedLen)
        {
            size = usedLen;
        }

        for (unsigned int i = 0; i < size; ++i)
        {
            data[i] = m_buffer[m_readMirrorIndex & m_mask];
            m_readMirrorIndex = (m_readMirrorIndex + 1) & m_maxMirrorBufferIndex;
        }

        return size;
    }

    /**
     * @brief read data from buffer without extracting it 从缓冲区读数据但不提取
     *
     * @param data [out] read data to save 待读取数据存储
     * @param size [in] read data size 待读取数据大小
     * @return return read data size 返回读取数据大小
     * @retval 0 buffer empty 缓冲区空
     * @retval [other] return read data size 返回读取数据大小
     */
    int peek(T *data, unsigned int size) override
    {
        if (isEmpty() || 0 == size)
        {
            return 0;
        }

        // check and set size to [1, n]
        const unsigned int usedLen = getUsedLen();
        if (size > usedLen)
        {
            size = usedLen;
        }

        unsigned int peekHead = m_readMirrorIndex;
        for (unsigned int i = 0; i < size; ++i)
        {
            data[i] = m_buffer[peekHead & m_mask];
            peekHead = (peekHead + 1) & m_maxMirrorBufferIndex;
        }

        return size;
    }

    /**
     * @brief skip buffer data 跳过指定大小缓冲区数据
     *
     * @param skipSize [in] skip data size 需要跳过的数据大小
     * @return skipped data size 跳过的数据大小
     */
    int skip(unsigned int skipSize) override
    {
        if (isEmpty() || 0 == skipSize)
        {
            return 0;
        }

        const unsigned int usedLen = getUsedLen();
        if (skipSize >= usedLen)
        {
            skipSize = usedLen;

            // skip all data
            m_readMirrorIndex = m_writeMirrotIndex;
        }
        else
        {
            m_readMirrorIndex = (m_readMirrorIndex + skipSize) & m_maxMirrorBufferIndex;
        }

        return skipSize;
    }

    /**
     * @brief is buffer full 缓冲区是否满
     *
     * @return true
     * @return false
     */
    bool isFull() const override
    {
        return m_writeMirrotIndex == (m_readMirrorIndex ^ m_maxBufferSize);
    }

    /**
     * @brief is buffer empty 缓冲区是否空
     *
     * @return true
     * @return false
     */
    bool isEmpty() const override
    {
        return m_writeMirrotIndex == m_readMirrorIndex;
    }

    /**
     * @brief get used length of buffer 获取缓冲区已使用大小
     *
     * @return return used length of buffer 返回缓冲区已使用大小
     */
    unsigned int getUsedLen() const override
    {
        return (m_writeMirrotIndex >= m_readMirrorIndex) ? (m_writeMirrotIndex - m_readMirrorIndex) : (m_writeMirrotIndex + 2 * m_maxBufferSize - m_readMirrorIndex);
    }

    /**
     * @brief get unused length of buffer 获取缓冲区未使用大小
     *
     * @return return unused length of buffer 返回缓冲区未使用大小
     */
    unsigned int getUnusedLen() const override
    {
        return m_maxBufferSize - getUsedLen();
    }

    /**
     * @brief get total size of buffer 获取缓冲区总大小
     *
     * @return return total size of buffer 返回缓冲区总大小
     */
    unsigned int getBufferSize() const override
    {
        return m_maxBufferSize;
    }

private:
    unsigned int m_readMirrorIndex{0};  ///< [0, 2n-1]
    unsigned int m_writeMirrotIndex{0}; ///< [0, 2n-1]

    const unsigned int m_maxBufferSize;        ///< n must power of two 必须为2的幂
    const unsigned int m_mask;                 ///< n-1
    const unsigned int m_maxMirrorBufferIndex; ///< 2n-1

    std::unique_ptr<T[]> m_buffer;
};

template <>
class RingBuffer<char> : public Buffer<char>
{
public:
    RingBuffer()
        : m_readMirrorIndex{0}
        , m_writeMirrotIndex{0}
        , m_maxBufferSize{4096} ///< must power of two
        , m_mask{m_maxBufferSize - 1}
        , m_maxMirrorBufferIndex{2 * m_maxBufferSize - 1}
        , m_buffer{new char[m_maxBufferSize]}
    {
    }

    RingBuffer(unsigned int maxBufferSize)
        : m_readMirrorIndex{0}
        , m_writeMirrotIndex{0}
        , m_maxBufferSize{(maxBufferSize && (0 == (maxBufferSize & (maxBufferSize - 1)))) ? maxBufferSize : nextPowerOf2(maxBufferSize)} ///< must power of two
        , m_mask{m_maxBufferSize - 1}
        , m_maxMirrorBufferIndex{2 * m_maxBufferSize - 1}
        , m_buffer{new char[m_maxBufferSize]}
    {
    }

    ~RingBuffer() override = default;

    RingBuffer(const RingBuffer &) = delete;
    RingBuffer &operator=(const RingBuffer &) = delete;

    int write(const char *data, unsigned int size) override
    {
        const unsigned int needWriteSize = size;
        // write data overflow, need move read index
        if (size > m_maxBufferSize) // size > n will overflow, need update size and move read index
        {
            data += (size - m_maxBufferSize); // only write last n
            size = m_maxBufferSize;
            m_readMirrorIndex = m_writeMirrotIndex;
        }
        else
        {
            // size > unusedLen will overflow, need move read index
            const unsigned int unusedLen = getUnusedLen();
            if (size > unusedLen)
            {
                const unsigned int skipSize = size - unusedLen;
                m_readMirrorIndex = (m_readMirrorIndex + skipSize) & m_maxMirrorBufferIndex;
            }
        }

        // memcpy
        const unsigned int writePos = m_writeMirrotIndex & m_mask;
        const unsigned int toEndSize = m_maxBufferSize - writePos;
        if (toEndSize >= size)
        {
            memcpy(m_buffer.get() + writePos, data, size);
        }
        else
        {
            // write index loop around
            memcpy(m_buffer.get() + writePos, data, toEndSize);
            memcpy(m_buffer.get(), data + toEndSize, size - toEndSize);
        }
        // update write index
        m_writeMirrotIndex = (m_writeMirrotIndex + size) & m_maxMirrorBufferIndex;

        return needWriteSize;
    }

    int read(char *data, unsigned int size) override
    {
        if (isEmpty() || 0 == size)
        {
            return 0;
        }

        // check and set size to [1, n]
        const unsigned int usedLen = getUsedLen();
        if (size > usedLen)
        {
            size = usedLen;
        }
        // memcpy
        const unsigned int readPos = m_readMirrorIndex & m_mask;
        const unsigned int toEndSize = m_maxBufferSize - readPos;
        if (toEndSize >= size)
        {
            memcpy(data, m_buffer.get() + readPos, size);
        }
        else
        {
            // read index loop around
            memcpy(data, m_buffer.get() + readPos, toEndSize);
            memcpy(data + toEndSize, m_buffer.get(), size - toEndSize);
        }
        // update read index
        m_readMirrorIndex = (m_readMirrorIndex + size) & m_maxMirrorBufferIndex;

        return size;
    }

    int peek(char *data, unsigned int size) override
    {
        if (isEmpty() || 0 == size)
        {
            return 0;
        }

        // check and set size to [1, n]
        unsigned int usedLen = getUsedLen();
        if (size > usedLen)
        {
            size = usedLen;
        }
        // memcpy
        const unsigned int readPos = m_readMirrorIndex & m_mask;
        const unsigned int toEndSize = m_maxBufferSize - readPos;
        if (toEndSize >= size)
        {
            memcpy(data, m_buffer.get() + readPos, size);
        }
        else
        {
            // read index loop around
            memcpy(data, m_buffer.get() + readPos, toEndSize);
            memcpy(data + toEndSize, m_buffer.get(), size - toEndSize);
        }

        return size;
    }

    int skip(unsigned int skipSize) override
    {
        if (isEmpty() || 0 == skipSize)
        {
            return 0;
        }

        unsigned int usedLen = getUsedLen();
        if (skipSize >= usedLen)
        {
            skipSize = usedLen;

            // skip all data
            m_readMirrorIndex = m_writeMirrotIndex;
        }
        else
        {
            m_readMirrorIndex = (m_readMirrorIndex + skipSize) & m_maxMirrorBufferIndex;
        }

        return skipSize;
    }

    bool isFull() const override
    {
        return m_writeMirrotIndex == (m_readMirrorIndex ^ m_maxBufferSize);
    }

    bool isEmpty() const override
    {
        return m_writeMirrotIndex == m_readMirrorIndex;
    }

    unsigned int getUsedLen() const override
    {
        return (m_writeMirrotIndex >= m_readMirrorIndex) ? (m_writeMirrotIndex - m_readMirrorIndex) : (m_writeMirrotIndex + 2 * m_maxBufferSize - m_readMirrorIndex);
    }

    unsigned int getUnusedLen() const override
    {
        return m_maxBufferSize - getUsedLen();
    }

    unsigned int getBufferSize() const override
    {
        return m_maxBufferSize;
    }

private:
    unsigned int m_readMirrorIndex{0};  ///< [0, 2n-1]
    unsigned int m_writeMirrotIndex{0}; ///< [0, 2n-1]

    const unsigned int m_maxBufferSize;        ///< n must power of two 必须为2的幂
    const unsigned int m_mask;                 ///< n-1
    const unsigned int m_maxMirrorBufferIndex; ///< 2n-1

    std::unique_ptr<char[]> m_buffer;
};
} // namespace itas109
#endif // __I_BUFFER_HPP__
