#include <gtest/gtest.h>
#include "infra/thread_operation/thread_sender/thread_sender.hpp"
#include "infra/thread_operation/thread_queue/thread_queue.hpp"
#include "infra/thread_operation/thread_message/thread_message.hpp"

using namespace device_reminder;

TEST(ThreadMessageSenderTest, EnqueueAddsToQueue) {
    auto queue = std::make_shared<ThreadQueue>(nullptr);
    auto message = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzer, std::vector<std::string>{"1"});
    ThreadSender sender(nullptr, queue, message);

    sender.send();
    auto res = queue->pop();
    ASSERT_NE(res, nullptr);
    EXPECT_EQ(res->type(), message->type());
    EXPECT_EQ(res->payload(), message->payload());
}
