#include <gtest/gtest.h>
#include "../ringbuffer.hpp"
#include <thread>
#include <memory>
#include <chrono>
#include <span>

class RingBufferTests : public testing::Test
{

protected:
    RingBufferTests() = default;

    void SetUp() override {}
    void TearDown() override {}
};


TEST_F(RingBufferTests, push_block)
{
    RingBuffer<float, 64> ringBuffer{};

    float tab[8]{1,2,3,4,5,6,7,8};

    ringBuffer.push_block(tab, 8);


    float* readTab = new float[4];

    ringBuffer.print_buffer();

    ringBuffer.pop(readTab, 4);

    for (uint16_t x = 0; x < 4; ++x)
    {
        std::cout << readTab[x] << ' ';
    }
    std::cout << std::endl;

    delete[] readTab;
}

TEST_F(RingBufferTests, MultiThreadedPush)
{
    RingBuffer<float, 64> ringBuffer{};

    auto producerWork = [&]() {
        for(int i = 0; i < 100; i++)
        {

            float data[8];
            for (int idx = 0; idx < 8; ++idx)
            {
                data[idx] = static_cast<float>(i+1);
            }

            ringBuffer.push_block(data, 8);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }  
    };

    EXPECT_EQ(1, 1);
}