#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "process_message_operation/process_message_receiver.hpp"
#include "thread_message_operation/thread_message_queue.hpp"
#include "infra/logger/i_logger.hpp"
#include <mqueue.h>
#include <thread>

using namespace device_reminder;

namespace {
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
};
} // namespace

static std::string unique_name(const std::string& base) {
    return "/" + base + std::to_string(::getpid()) + std::to_string(::time(nullptr));
}

TEST(MessageReceiverTest, ReceivesAndPushes) {
    std::string name = unique_name("receiver_test_");
    auto queue = std::make_shared<ThreadMessageQueue>();
    MockLogger logger;
    ProcessMessageReceiver receiver(name, queue, std::shared_ptr<ILogger>(&logger, [](ILogger*){}));
    std::thread th{std::ref(receiver)};

    mqd_t mq = mq_open(name.c_str(), O_WRONLY);
    ASSERT_NE(mq, static_cast<mqd_t>(-1));
    ProcessMessage msg{MessageType::BuzzerOn, true};
    mq_send(mq, reinterpret_cast<const char*>(&msg), PROCESS_MESSAGE_SIZE, 0);
    mq_close(mq);

    auto res = queue->pop();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->type_, msg.type_);

    receiver.stop();
    th.join();
}
