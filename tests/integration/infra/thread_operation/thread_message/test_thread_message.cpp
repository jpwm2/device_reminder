#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/thread_operation/thread_message/thread_message.hpp"

using namespace device_reminder;

TEST(ThreadMessageIntegrationTest, HandlesValidInput) {
    ThreadMessage msg(ThreadMessageType::StartBuzzing, {"1"});

    EXPECT_EQ(msg.type(), ThreadMessageType::StartBuzzing);
    std::vector<std::string> expected{"1"};
    EXPECT_EQ(msg.payload(), expected);

    auto copy = msg.clone();
    ASSERT_NE(copy, nullptr);
    EXPECT_EQ(copy->type(), msg.type());
    EXPECT_EQ(copy->payload(), msg.payload());

    EXPECT_EQ(msg.to_string(), std::string("ThreadMessage{7,[1]}"));
}

TEST(ThreadMessageIntegrationTest, HandlesInvalidEnum) {
    ThreadMessage msg(static_cast<ThreadMessageType>(999), {"x"});

    EXPECT_EQ(static_cast<int>(msg.type()), 999);
    std::vector<std::string> expected{"x"};
    EXPECT_EQ(msg.payload(), expected);
    EXPECT_EQ(msg.to_string(), std::string("ThreadMessage{999,[x]}"));
}

