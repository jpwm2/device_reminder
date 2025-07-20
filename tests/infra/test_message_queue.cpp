#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/message_operator/message_queue.hpp"

using namespace device_reminder;

static std::string unique_queue_name(const std::string& base) {
    return "/" + base + std::to_string(::getpid()) + std::to_string(::time(nullptr));
}

TEST(MessageQueueTest, PushAndPop) {
    std::string name = unique_queue_name("mq_test_");
    MessageQueue mq(name, true);
    Message msg{MessageType::BuzzerOn, true};
    EXPECT_TRUE(mq.push(msg));
    auto res = mq.pop();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->type_, msg.type_);
    EXPECT_EQ(res->payload_, msg.payload_);
    mq.close();
    EXPECT_FALSE(mq.is_open());
}
