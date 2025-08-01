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
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

class MockQueue : public IThreadQueue {
public:
    MOCK_METHOD(void, push, (std::shared_ptr<IThreadMessage>), (override));
    MOCK_METHOD(std::shared_ptr<IThreadMessage>, pop, (), (override));
    MOCK_METHOD(size_t, size, (), (const, override));
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

TEST(ThreadSenderTest, ConstructorAcceptsNullLogger) {
    auto queue = std::make_shared<ThreadQueue>(nullptr);
    auto message = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing,
                                                   std::vector<std::string>{"1"});
    EXPECT_NO_THROW({ ThreadSender sender(nullptr, queue, message); });
}

TEST(ThreadSenderTest, ConstructorHandlesNullDependencies) {
    auto message = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing,
                                                   std::vector<std::string>{"1"});
    EXPECT_NO_THROW({ ThreadSender sender(nullptr, nullptr, message); });
}

TEST(ThreadSenderTest, ConstructorLogsWhenLoggerProvided) {
    NiceMock<MockLogger> logger;
    EXPECT_CALL(logger, info(testing::_)).Times(1);
    auto queue = std::make_shared<ThreadQueue>(nullptr);
    auto message = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing,
                                                   std::vector<std::string>{"1"});
    ThreadSender sender(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), queue, message);
}

TEST(ThreadSenderTest, ConstructorLogsWithoutErrorsWhenQueueNull) {
    NiceMock<MockLogger> logger;
    EXPECT_CALL(logger, info(testing::_)).Times(1);
    EXPECT_CALL(logger, error(testing::_)).Times(0);
    ThreadSender sender(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), nullptr, nullptr);
}

TEST(ThreadSenderTest, SendLogsErrorWhenQueueNull) {
    NiceMock<MockLogger> logger;
    auto message = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing,
                                                   std::vector<std::string>{"1"});
    ThreadSender sender(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), nullptr, message);
    EXPECT_CALL(logger, error(testing::_)).Times(1);
    sender.send();
}

TEST(ThreadSenderTest, SendLogsErrorWhenMessageNull) {
    NiceMock<MockLogger> logger;
    auto queue = std::make_shared<ThreadQueue>(nullptr);
    ThreadSender sender(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), queue, nullptr);
    EXPECT_CALL(logger, error(testing::_)).Times(1);
    sender.send();
}

TEST(ThreadSenderTest, SendUsesMockQueue) {
    NiceMock<MockQueue> queue;
    NiceMock<MockLogger> logger;
    auto message = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing,
                                                   std::vector<std::string>{"1"});
    ThreadSender sender(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                        std::shared_ptr<IThreadQueue>(&queue, [](IThreadQueue*){}),
                        message);
    EXPECT_CALL(queue, push(testing::_)).Times(1);
    sender.send();
}

TEST(ThreadSenderTest, SendLogsErrorWhenQueueAndMessageNull) {
    NiceMock<MockLogger> logger;
    ThreadSender sender(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), nullptr, nullptr);
    EXPECT_CALL(logger, error(testing::_)).Times(1);
    sender.send();
}
