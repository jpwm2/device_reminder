#include <gtest/gtest.h>

#include "infra/message/message.hpp"

using namespace device_reminder;

TEST(MessageTest, StoresTypeAndPayload) {
    Message msg(MessageType::StartBuzzer, {"a", "b"}, nullptr);
    EXPECT_EQ(msg.type(), MessageType::StartBuzzer);
    auto p = msg.payload();
    ASSERT_EQ(p.size(), 2);
    EXPECT_EQ(p[0], "a");
    EXPECT_FALSE(msg.to_string().empty());
}
