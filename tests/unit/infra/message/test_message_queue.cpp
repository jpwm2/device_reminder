#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "infra/logger.hpp"
#include "infra/message/message.hpp"
#include "infra/message/message_queue.hpp"

using namespace device_reminder;
using ::testing::NiceMock;

namespace {
class MockLogger : public ILogger {
public:
  MOCK_METHOD(void, info, (const std::string &), (override));
  MOCK_METHOD(void, error, (const std::string &), (override));
  MOCK_METHOD(void, warn, (const std::string &), (override));
};
} // namespace

TEST(ThreadQueueTest, PushPopWorks) {
  auto logger = std::make_shared<NiceMock<MockLogger>>();
  ThreadQueue q(logger);
  auto msg = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing,
                                             std::vector<std::string>{"1"});
  q.push(msg);
  auto res = q.pop();
  ASSERT_NE(res, nullptr);
  EXPECT_EQ(res->type(), msg->type());
  EXPECT_EQ(res->payload(), msg->payload());
}

TEST(ThreadQueueTest, PopOnEmptyReturnsNullptr) {
  ThreadQueue q(nullptr);
  auto res = q.pop();
  EXPECT_EQ(res, nullptr);
}

TEST(ThreadQueueTest, SizeReflectsQueueState) {
  ThreadQueue q(nullptr);
  EXPECT_EQ(q.size(), 0u);
  q.push(std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing,
                                         std::vector<std::string>{}));
  EXPECT_EQ(q.size(), 1u);
}


TEST(ThreadQueueTest, ConstructorAcceptsNullLogger) {
  ThreadQueue q(nullptr);
  EXPECT_EQ(q.size(), 0u);
}

TEST(ThreadQueueTest, PushLogsWhenLoggerProvided) {
  auto logger = std::make_shared<NiceMock<MockLogger>>();
  EXPECT_CALL(*logger, info("ThreadQueue push"));
  ThreadQueue q(logger);
  auto msg = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing,
                                             std::vector<std::string>{"2"});
  q.push(msg);
  auto res = q.pop();
  ASSERT_NE(res, nullptr);
  EXPECT_EQ(res->type(), msg->type());
  EXPECT_EQ(res->payload(), msg->payload());
}

TEST(ThreadQueueTest, PushMultipleMaintainsFIFO) {
  ThreadQueue q(nullptr);
  auto msg1 = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing,
                                              std::vector<std::string>{"a"});
  auto msg2 = std::make_shared<ThreadMessage>(ThreadMessageType::StopBuzzing,
                                              std::vector<std::string>{"b"});
  q.push(msg1);
  q.push(msg2);
  auto first = q.pop();
  auto second = q.pop();
  ASSERT_NE(first, nullptr);
  ASSERT_NE(second, nullptr);
  EXPECT_EQ(first->type(), msg1->type());
  EXPECT_EQ(second->type(), msg2->type());
}

TEST(ThreadQueueTest, PushNullMessageHandled) {
  ThreadQueue q(nullptr);
  q.push(nullptr);
  EXPECT_EQ(q.size(), 1u);
  auto res = q.pop();
  EXPECT_EQ(res, nullptr);
}

TEST(ThreadQueueTest, PushNullMessageWithMockLogger) {
  auto logger = std::make_shared<NiceMock<MockLogger>>();
  EXPECT_CALL(*logger, info("ThreadQueue push"));
  ThreadQueue q(logger);
  q.push(nullptr);
  auto res = q.pop();
  EXPECT_EQ(res, nullptr);
}

