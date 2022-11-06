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
template <typename T> class Buffer
{
public:
    Buffer(){};
    virtual ~Buffer(){};

    /**
     * @brief write data to buffer 向缓冲区写数据
     *
     * @param data [in] write data 待写入数据
     * @param size [in] write data size 待写入大小
     * @return return write data size 返回写入数据大小
     */
    virtual int write(const T *data, unsigned int size) = 0;
    /**
     * @brief read data from buffer 从缓冲区读数据
     *
     * @param data [out] read data to save 待读取数据存储
     * @param size [in] read data size 待读取数据大小
     * @return return read data size 返回读取数据大小
     */
    virtual int read(T *data, unsigned int size) = 0;

    /**
     * @brief is buffer full 缓冲区是否满
     *
     * @return true
     * @return false
     */
    virtual bool isFull() = 0;
    /**
     * @brief is buffer empty 缓冲区是否空
     *
     * @return true
     * @return false
     */
    virtual bool isEmpty() = 0;

    /**
     * @brief get used length of buffer 获取缓冲区已使用大小
     *
     * @return return used length of buffer 返回缓冲区已使用大小
     */
    virtual unsigned int getUsedLen() = 0;
    /**
     * @brief get unused length of buffer 获取缓冲区未使用大小
     *
     * @return return unused length of buffer 返回缓冲区未使用大小
     */
    virtual unsigned int getUnusedLen() = 0;
    /**
     * @brief get total size of buffer 获取缓冲区总大小
     *
     * @return return total size of buffer 返回缓冲区总大小
     */
    virtual unsigned int getBufferSize() = 0;
};

/**
 * @brief RingBuffer MSB(mirroring significant bit) 环形缓冲区(镜像指示位方式)
 *
 * @tparam T
 */
template <typename T> class RingBuffer : public Buffer<T>
{
public:
    /**
     * @brief Construct a new Ring Buffer object 构造函数
     *
     */
    RingBuffer()
        : m_head(0)
        , m_tail(0)
        , m_maxBufferSize(4096) ///< must power of two
        , m_maxMirrorBufferIndex(2 * m_maxBufferSize - 1)
        , m_buffer(new T[m_maxBufferSize])
    {
    }

    /**
     * @brief Construct a new Ring Buffer object 通过缓冲区大小构造
     *
     * @param maxBufferSize [in] buffer size 缓冲区大小
     */
    RingBuffer(unsigned int maxBufferSize)
        : m_head(0)
        , m_tail(0)
        , m_maxBufferSize((maxBufferSize && (0 == (maxBufferSize & (maxBufferSize - 1)))) ? maxBufferSize : nextPowerOf2(maxBufferSize)) ///< must power of two
        , m_maxMirrorBufferIndex(2 * m_maxBufferSize - 1)
        , m_buffer(new T[m_maxBufferSize])
    {
    }

    /**
     * @brief Destroy the Ring Buffer object 析构函数
     *
     */
    virtual ~RingBuffer()
    {
        if (m_buffer)
        {
            delete[] m_buffer;
            m_buffer = NULL;
        }
    }

    /**
     * @brief write data to buffer 向缓冲区写数据
     *
     * @param data [in] write data 待写入数据
     * @param size [in] write data size 待写入大小
     * @return return write data size 返回写入数据大小
     */
    virtual int write(const T *data, unsigned int size)
    {
        for (unsigned int i = 0; i < size; i++)
        {
            m_buffer[m_tail & (m_maxBufferSize - 1)] = data[i];
            if (isFull())
            {
                m_head = (m_head + 1) & m_maxMirrorBufferIndex; ///< head and tail pointers incrementation is done modulo 2*size
            }
            m_tail = (m_tail + 1) & m_maxMirrorBufferIndex;
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
    virtual int read(T *data, unsigned int size)
    {
        if (isEmpty())
        {
            return 0;
        }

        if (size > m_maxBufferSize)
        {
            size = m_maxBufferSize;
        }

        for (unsigned int i = 0; i < size; i++)
        {
            *data = m_buffer[m_head & (m_maxBufferSize - 1)];
            data++;
            m_head = (m_head + 1) & m_maxMirrorBufferIndex;
        }

        return size;
    }

    /**
     * @brief is buffer full 缓冲区是否满
     *
     * @return true
     * @return false
     */
    bool isFull()
    {
        return m_tail == (m_head ^ m_maxBufferSize);
    }

    /**
     * @brief is buffer empty 缓冲区是否空
     *
     * @return true
     * @return false
     */
    virtual bool isEmpty()
    {
        return m_tail == m_head;
    }

    /**
     * @brief get used length of buffer 获取缓冲区已使用大小
     *
     * @return return used length of buffer 返回缓冲区已使用大小
     */
    virtual unsigned int getUsedLen()
    {
        return (m_tail >= m_head) ? (m_tail - m_head) : (m_tail + 2 * m_maxBufferSize - m_head);
    }

    /**
     * @brief get unused length of buffer 获取缓冲区未使用大小
     *
     * @return return unused length of buffer 返回缓冲区未使用大小
     */
    virtual unsigned int getUnusedLen()
    {
        return m_maxBufferSize - getUsedLen();
    }

    /**
     * @brief get total size of buffer 获取缓冲区总大小
     *
     * @return return total size of buffer 返回缓冲区总大小
     */
    virtual unsigned int getBufferSize()
    {
        return m_maxBufferSize;
    }

private:
    unsigned int m_head;
    unsigned int m_tail;

    unsigned int m_maxBufferSize;        ///< must power of two 必须为2的幂
    unsigned int m_maxMirrorBufferIndex; ///< 2 * m_maxBuffSize - 1
    T *m_buffer;
};
} // namespace itas109
#endif // __I_BUFFER_HPP__
