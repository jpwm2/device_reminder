#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/logger.hpp"
#include "infra/message/message.hpp"
#include "infra/message/message_queue.hpp"
#include "infra/message/message_type.hpp"
#include "infra/message/thread_sender.hpp"

using namespace device_reminder;
using ::testing::NiceMock;

namespace {
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};
} // namespace

TEST(ThreadSenderTest, SendPushesMessageToQueue) {
    NiceMock<MockLogger> logger;
    auto queue = std::make_shared<MessageQueue>(std::shared_ptr<ILogger>(&logger, [](ILogger*){}));
    auto message = std::make_shared<Message>(
        MessageType::StartBuzzing,
        std::vector<std::string>{"1"},
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}));

    ThreadSender sender(std::shared_ptr<ILogger>(&logger, [](ILogger*){}));

    sender.send(queue, message);

    auto res = queue->pop();
    ASSERT_NE(res, nullptr);
    EXPECT_EQ(res->type(), message->type());
    EXPECT_EQ(res->payload(), message->payload());
}
