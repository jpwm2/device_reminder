#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/thread_operation/thread_sender/thread_sender.hpp"
#include "infra/thread_operation/thread_queue/thread_queue.hpp"
#include "infra/thread_operation/thread_message/thread_message.hpp"
#include "infra/logger/i_logger.hpp"

using namespace device_reminder;
using ::testing::NiceMock;

namespace {
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
};
} // namespace

TEST(ThreadSenderTest, SendPushesMessageToQueue) {
    auto queue = std::make_shared<ThreadQueue>(nullptr);
    NiceMock<MockLogger> logger;
    auto message = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzer, std::vector<std::string>{"1"});

    ThreadSender sender(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), queue, message);

    sender.send();

    auto res = queue->pop();
    ASSERT_NE(res, nullptr);
    EXPECT_EQ(res->type(), message->type());
    EXPECT_EQ(res->payload(), message->payload());
}
