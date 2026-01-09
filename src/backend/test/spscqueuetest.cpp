#include <gtest/gtest.h>
#include "../spscqueue.hpp"
#include <thread>
#include <memory>
#include <chrono>
#include <span>

class SpscQueueTests : public testing::Test
{

protected:
    SpscQueueTests() = default;

    void SetUp() override {}
    void TearDown() override {}
};


TEST_F(SpscQueueTests, try_push)
{

    SpscQueue<float, 3> q;
    EXPECT_EQ(1,1);
}