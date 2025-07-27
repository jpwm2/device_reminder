#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "thread_operation/thread_sender.hpp"
#include "thread_operation/thread_queue/thread_queue.hpp"
#include "thread_operation/thread_message/thread_message.hpp"
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
    auto queue = std::make_shared<ThreadQueue<std::shared_ptr<IThreadMessage>>>();
    NiceMock<MockLogger> logger;
    auto message = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzer, true);

    ThreadSender sender(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), queue, message);

    sender.send();

    auto res = queue->pop();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ((*res)->type(), message->type());
    EXPECT_EQ((*res)->payload(), message->payload());
}
