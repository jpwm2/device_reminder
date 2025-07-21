#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "process_message_operation/process_message_sender.hpp"
#include <mqueue.h>

using namespace device_reminder;

static std::string unique_name(const std::string& base) {
    return "/" + base + std::to_string(::getpid()) + std::to_string(::time(nullptr));
}

TEST(MessageSenderTest, EnqueueSendsMessage) {
    std::string name = unique_name("sender_test_");
    ProcessMessageSender sender(name, 5);

    ProcessMessage msg{MessageType::BuzzerOn, true};
    ASSERT_TRUE(sender.enqueue(msg));

    mqd_t mq = mq_open(name.c_str(), O_RDONLY);
    ASSERT_NE(mq, static_cast<mqd_t>(-1));
    ProcessMessage out{};
    ssize_t n = mq_receive(mq, reinterpret_cast<char*>(&out), PROCESS_MESSAGE_SIZE, nullptr);
    EXPECT_EQ(n, (ssize_t)PROCESS_MESSAGE_SIZE);
    EXPECT_EQ(out.type_, msg.type_);
    mq_close(mq);

    sender.stop();
}
