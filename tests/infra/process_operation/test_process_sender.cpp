#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/process_operation/process_sender/process_sender.hpp"
#include "infra/process_operation/process_queue/process_queue.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/process_operation/message_codec/message_codec.hpp"
#include "infra/logger/logger.hpp"
#include <mqueue.h>

using namespace device_reminder;

static std::string unique_name(const std::string& base) {
    return "/" + base + std::to_string(::getpid()) + std::to_string(::time(nullptr));
}

TEST(MessageSenderTest, EnqueueSendsMessage) {
    std::string name = unique_name("sender_test_");
    auto logger = std::make_shared<Logger>();
    auto codec = std::make_shared<MessageCodec>();
    auto queue = std::make_shared<ProcessQueue>(logger, codec, name);
    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{"1"});
    ProcessSender sender(queue, msg);

    sender.send();

    mqd_t mq = mq_open(name.c_str(), O_RDONLY);
    ASSERT_NE(mq, static_cast<mqd_t>(-1));
    std::vector<uint8_t> buf(512);
    ssize_t n = mq_receive(mq, reinterpret_cast<char*>(buf.data()), 512, nullptr);
    EXPECT_GT(n, 0);
    mq_close(mq);
}
