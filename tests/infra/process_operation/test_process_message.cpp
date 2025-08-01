#include <gtest/gtest.h>
#include "infra/process_operation/process_message/process_message.hpp"

using namespace device_reminder;

TEST(ProcessMessageTest, ConstructorStoresValues) {
    ProcessMessage msg(ProcessMessageType::StartBuzzing, {"arg1", "arg2"});
    EXPECT_EQ(msg.type(), ProcessMessageType::StartBuzzing);
    std::vector<std::string> expected{"arg1", "arg2"};
    EXPECT_EQ(msg.payload(), expected);
}

TEST(ProcessMessageTest, HandlesEmptyPayload) {
    ProcessMessage msg(ProcessMessageType::ScanTimeout, {});
    EXPECT_EQ(msg.type(), ProcessMessageType::ScanTimeout);
    EXPECT_TRUE(msg.payload().empty());
}

TEST(ProcessMessageTest, HandlesInvalidEnum) {
    ProcessMessage msg(static_cast<ProcessMessageType>(999), {"x"});
    EXPECT_EQ(static_cast<int>(msg.type()), 999);
    std::vector<std::string> expected{"x"};
    EXPECT_EQ(msg.payload(), expected);
    EXPECT_EQ(msg.to_string(), std::string("ProcessMessage{999,[x]}"));
}

TEST(ProcessMessageTest, CloneCreatesEqualCopy) {
    ProcessMessage original(ProcessMessageType::StopBuzzing, {"stop"});
    auto copy = original.clone();
    ASSERT_NE(copy, nullptr);
    EXPECT_EQ(copy->type(), original.type());
    EXPECT_EQ(copy->payload(), original.payload());
}

TEST(ProcessMessageTest, ToStringFormatsCorrectly) {
    ProcessMessage msg(ProcessMessageType::StartBuzzing, {"1"});
    EXPECT_EQ(msg.to_string(), std::string("ProcessMessage{5,[1]}"));
}
