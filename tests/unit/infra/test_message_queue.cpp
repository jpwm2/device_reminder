#include <gtest/gtest.h>

#include "infra/message/message_queue.hpp"
#include "infra/message/message.hpp"

using namespace device_reminder;

TEST(MessageQueueTest, PushAndPop) {
    MessageQueue queue(nullptr);
    auto msg = std::make_shared<Message>(MessageType::DeviceFound, std::vector<std::string>{"x"}, nullptr);
    queue.push(msg);
    auto out = queue.pop();
    EXPECT_EQ(out, msg);
}
