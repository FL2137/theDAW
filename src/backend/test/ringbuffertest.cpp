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
    RingBuffer<float, 8> ringBuffer{};

    float tab[8]{1,2,3,4,5,6,7,8};

    float readTab[2]{};

    float expectedValue = 1;
    // check if the whole tab pushed and popped from the buffer correctly
    for (uint16_t x = 0; x < 4; ++x)
    {
        ringBuffer.push_block(tab + (x * 2), 2);
        ringBuffer.pop(readTab, 2);

        EXPECT_EQ(readTab[0], expectedValue);
        EXPECT_EQ(readTab[1], expectedValue + 1.f);
        expectedValue += 2.f;
    }

    // check if the values are pushed and popped correctly after exceeding the ring buffers capacity
    expectedValue = 1.f;
    ringBuffer.push_block(tab, 2);
    ringBuffer.pop(readTab, 2);

    EXPECT_EQ(readTab[0], expectedValue);
    EXPECT_EQ(readTab[1], expectedValue + 1.f);
}

TEST_F(RingBufferTests, MultiThreadedPush)
{
    RingBuffer<float, 64> ringBuffer{};

    auto producerWork = [&]() {
        for(int i = 0; i < 3
            ; i++)
        {

            float data[8];
            for (int idx = 0; idx < 8; ++idx)
            {
                data[idx] = static_cast<float>(i+1);
            }

            ringBuffer.push_block(data, 8);
        }  
    };

    float readBuffer[24]{0.0f};

    auto consoomerWork = [&]() {
        for (int i = 0; i < 3; i++)
        {
            ringBuffer.pop(readBuffer + (i * 8), 8);
        }
    };

    std::thread producerThread(producerWork);
    std::thread consoomerThread(consoomerWork);

    
    producerThread.join();
    consoomerThread.join();

    for (int idx = 0; idx < 24; idx++)
    {
        std::cout << readBuffer[idx] << std::endl;
    }

    EXPECT_EQ(readBuffer[10], 100.f);
}