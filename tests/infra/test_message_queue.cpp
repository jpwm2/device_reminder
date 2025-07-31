#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/process_operation/process_queue/process_queue.hpp"
#include "infra/process_operation/message_codec/message_codec.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/logger/logger.hpp"

using namespace device_reminder;

static std::string unique_queue_name(const std::string& base) {
    return "/" + base + std::to_string(::getpid()) + std::to_string(::time(nullptr));
}

TEST(MessageQueueTest, PushAndPop) {
    std::string name = unique_queue_name("mq_test_");
    auto logger = std::make_shared<Logger>();
    auto codec = std::make_shared<MessageCodec>();
    ProcessQueue mq(logger, codec, name);
    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzer,
                                               std::vector<std::string>{"1"});
    mq.push(msg);
    auto res = mq.pop();
    ASSERT_NE(res, nullptr);
    EXPECT_EQ(res->type(), msg->type());
    EXPECT_EQ(res->payload(), msg->payload());
}
