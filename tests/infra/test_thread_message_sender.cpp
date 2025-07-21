#include <gtest/gtest.h>
#include "thread_message_operation/thread_message_sender.hpp"
#include "thread_message_operation/thread_message_queue.hpp"

using namespace device_reminder;

TEST(ThreadMessageSenderTest, EnqueueAddsToQueue) {
    auto queue = std::make_shared<ThreadMessageQueue>();
    ThreadMessageSender sender(queue);

    ThreadMessage msg{MessageType::BuzzerOn, true};
    ASSERT_TRUE(sender.enqueue(msg));

    auto res = queue->pop();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->type_, msg.type_);
    EXPECT_EQ(res->payload_, msg.payload_);

    sender.stop();
}
