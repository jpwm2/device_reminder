#include <gtest/gtest.h>
#include "infra/thread_operation/thread_message/thread_message.hpp"

using namespace device_reminder;

TEST(ThreadMessageTest, ConstructorStoresValues) {
    ThreadMessage msg(ThreadMessageType::StartBuzzing, {"1"});
    EXPECT_EQ(msg.type(), ThreadMessageType::StartBuzzing);
    std::vector<std::string> expected{"1"};
    EXPECT_EQ(msg.payload(), expected);
}

TEST(ThreadMessageTest, HandlesUnknownType) {
    ThreadMessage msg(ThreadMessageType::Unknown, {});
    EXPECT_EQ(msg.type(), ThreadMessageType::Unknown);
    EXPECT_TRUE(msg.payload().empty());
}

TEST(ThreadMessageTest, HandlesInvalidEnum) {
    ThreadMessage msg(static_cast<ThreadMessageType>(999), {"x"});
    EXPECT_EQ(static_cast<int>(msg.type()), 999);
    std::vector<std::string> expected{"x"};
    EXPECT_EQ(msg.payload(), expected);
    EXPECT_EQ(msg.to_string(), std::string("ThreadMessage{999,[x]}"));
}

TEST(ThreadMessageTest, CloneCreatesEqualCopy) {
    ThreadMessage original(ThreadMessageType::RespondDeviceFound, {"phone", "watch"});
    auto copy = original.clone();
    ASSERT_NE(copy, nullptr);
    EXPECT_EQ(copy->type(), original.type());
    EXPECT_EQ(copy->payload(), original.payload());
}

TEST(ThreadMessageTest, ToStringFormatsCorrectly) {
    ThreadMessage msg(ThreadMessageType::StartBuzzing, {"1"});
    EXPECT_EQ(msg.to_string(), std::string("ThreadMessage{7,[1]}"));
}

