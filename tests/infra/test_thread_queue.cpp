#include <gtest/gtest.h>
#include "thread_operation/thread_queue/thread_queue.hpp"

using namespace device_reminder;

TEST(ThreadQueueTest, PushPopWorks) {
    ThreadQueue<int> q;
    q.push(42);
    auto res = q.pop();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), 42);
}

TEST(ThreadQueueTest, PopTimeoutReturnsNullopt) {
    ThreadQueue<int> q;
    auto res = q.pop(10);
    EXPECT_FALSE(res.has_value());
}

TEST(ThreadQueueTest, SizeAndClearWork) {
    ThreadQueue<int> q;
    EXPECT_TRUE(q.empty());
    q.push(1);
    q.push(2);
    EXPECT_EQ(q.size(), 2u);
    EXPECT_FALSE(q.empty());
    q.clear();
    EXPECT_TRUE(q.empty());
    EXPECT_EQ(q.size(), 0u);
}
