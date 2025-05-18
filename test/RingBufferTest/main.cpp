// doctest
#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_NO_POSIX_SIGNALS // fixed SIGSTKSZ compile error on ubuntu 22
#include "doctest.h"

#include "CSerialPort/ibuffer.hpp"

typedef char test_t;

TEST_SUITE("RingBuffer Tests")
{
    TEST_CASE("Default Constructor")
    {
        itas109::RingBuffer<test_t> rb;
        CHECK(rb.getBufferSize() == 4096);
        CHECK(rb.isEmpty());
        CHECK_FALSE(rb.isFull());
    }

    TEST_CASE("Constructor with size adjustment")
    {
        SUBCASE("Non-power-of-two size")
        {
            itas109::RingBuffer<test_t> rb(3000);
            CHECK(rb.getBufferSize() == 4096);
        }

        SUBCASE("Exact power-of-two size")
        {
            itas109::RingBuffer<test_t> rb(8192);
            CHECK(rb.getBufferSize() == 8192);
        }
    }

    TEST_CASE("Basic Read/Write operations")
    {
        itas109::RingBuffer<test_t> rb(4);
        test_t data[4] = {1, 2, 3, 4};
        test_t read_buf[4] = {0};

        SUBCASE("Single element write/read")
        {
            CHECK(1 == rb.write(data, 1));
            CHECK(1 == rb.read(read_buf, 1));
            CHECK(read_buf[0] == 1);
        }

        SUBCASE("Full buffer cycle")
        {
            // 写满缓冲区
            CHECK(4 == rb.write(data, 4));
            CHECK(rb.isFull());

            // 读空缓冲区
            CHECK(4 == rb.read(read_buf, 4));
            CHECK(rb.isEmpty());
            CHECK(read_buf[0] == data[0]);
            CHECK(read_buf[1] == data[1]);
            CHECK(read_buf[2] == data[2]);
            CHECK(read_buf[3] == data[3]);
        }

        SUBCASE("Overwrite behavior > unusedLen")
        {
            // 第一次填满
            rb.write(data, 4);
            CHECK(rb.isFull());

            // 覆盖写入2个新元素
            test_t new_data[2] = {5, 6};
            CHECK(2 == rb.write(new_data, 2));
            CHECK(rb.getUsedLen() == 4); // 始终保持满状态

            // 读取时应跳过头两个元素
            CHECK(4 == rb.read(read_buf, 4));
            CHECK(read_buf[0] == 3);
            CHECK(read_buf[1] == 4);
            CHECK(read_buf[2] == 5);
            CHECK(read_buf[3] == 6);
        }

        SUBCASE("Overwrite behavior > n")
        {
            test_t new_data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
            CHECK(9 == rb.write(new_data, 9));
            CHECK(rb.getUsedLen() == 4); // 始终保持满状态

            // 读取时应跳过头两个元素
            CHECK(4 == rb.read(read_buf, 4));
            CHECK(read_buf[0] == 6);
            CHECK(read_buf[1] == 7);
            CHECK(read_buf[2] == 8);
            CHECK(read_buf[3] == 9);
        }
    }

    TEST_CASE("Peek/Skip operations")
    {
        itas109::RingBuffer<test_t> rb(4);
        test_t data[4] = {1, 2, 3, 4};
        test_t peek_buf[4] = {0};

        rb.write(data, 4);

        SUBCASE("Peek without read")
        {
            CHECK(4 == rb.peek(peek_buf, 4));
            CHECK(peek_buf[0] == data[0]);
            CHECK(peek_buf[1] == data[1]);
            CHECK(peek_buf[2] == data[2]);
            CHECK(peek_buf[3] == data[3]);
            CHECK(rb.getUsedLen() == 4); // peek 不应影响指针
        }

        SUBCASE("Partial skip")
        {
            CHECK(2 == rb.skip(2));
            CHECK(rb.getUsedLen() == 2);
            CHECK(2 == rb.peek(peek_buf, 2));
            CHECK(peek_buf[0] == 3);
            CHECK(peek_buf[1] == 4);
        }

        SUBCASE("Over-skip")
        {
            CHECK(4 == rb.skip(5)); // 跳过的元素数不超过缓冲区大小
            CHECK(rb.isEmpty());
        }
    }

    TEST_CASE("Edge Cases")
    {
        SUBCASE("Empty buffer behavior")
        {
            itas109::RingBuffer<test_t> rb(4);
            test_t data;
            CHECK(0 == rb.read(&data, 1));
            CHECK(0 == rb.skip(1));
        }

        SUBCASE("Full buffer with wraparound")
        {
            itas109::RingBuffer<test_t> rb(4);
            test_t data[6] = {1, 2, 3, 4, 5, 6};
            test_t read_buf[4];

            // 触发缓冲区覆盖
            CHECK(6 == rb.write(data, 6)); // 覆盖前2个元素
            CHECK(rb.getBufferSize() == 4);
            CHECK(rb.isFull());

            // 应该读取最后4个元素
            CHECK(4 == rb.read(read_buf, 4));
            CHECK(read_buf[0] == 3);
            CHECK(read_buf[1] == 4);
            CHECK(read_buf[2] == 5);
            CHECK(read_buf[3] == 6);
        }
    }

    TEST_CASE("Utility functions")
    {
        itas109::RingBuffer<test_t> rb(4);
        test_t data[4] = {1, 2, 3, 4};

        SUBCASE("Initial state")
        {
            CHECK(rb.getUsedLen() == 0);
            CHECK(rb.getUnusedLen() == 4);
        }

        SUBCASE("After partial write")
        {
            rb.write(data, 1);
            CHECK(rb.getUsedLen() == 1);
            CHECK(rb.getUnusedLen() == 3);
        }

        SUBCASE("After full write")
        {
            rb.write(data, 4);
            CHECK(rb.getUsedLen() == 4);
            CHECK(rb.getUnusedLen() == 0);
        }
    }

    TEST_CASE("Performance 1MB")
    {
        const int dataSize = 1 * 1024 * 1024; // 1MB
        itas109::RingBuffer<test_t> rb(dataSize);
        test_t *data = new test_t[dataSize];
        memset(data, 1, dataSize);

        rb.write(data, dataSize);

        delete[] data;
    }

    TEST_CASE("Performance 10w128B")
    {
        itas109::RingBuffer<test_t> rb(4096);
        test_t data[128];
        memset(data, 1, 128);

        for (int i = 0; i < 100000; ++i)
        {
            rb.write(data, 128);
            rb.skip(128);
        }
    }
}

int main(int argc, char **argv)
{
    doctest::Context context;
    context.applyCommandLine(argc, argv);
    context.setOption("duration", true); 

     context.addFilter("test-case", "Performance 1MB");

    int res = context.run();

#ifdef _DEBUG
    printf("\nPress Enter To Continue...\n");
    (void)getchar();
#endif

    return 0;
}