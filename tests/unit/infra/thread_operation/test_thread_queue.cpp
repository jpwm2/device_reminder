#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdexcept>

#include "infra/logger/i_logger.hpp"
#include "infra/message/message.hpp"
#include "infra/thread_operation/thread_queue/thread_queue.hpp"

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
  auto msg = std::make_shared<Message>(MessageType::StartBuzzer,
                                       std::vector<std::string>{"1"}, logger);
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
  auto msg_logger = std::make_shared<NiceMock<MockLogger>>();
  q.push(std::make_shared<Message>(MessageType::StartBuzzer,
                                   std::vector<std::string>{}, msg_logger));
  EXPECT_EQ(q.size(), 1u);
}


TEST(ThreadQueueTest, ConstructorAcceptsNullLogger) {
  ThreadQueue q(nullptr);
  EXPECT_EQ(q.size(), 0u);
}

TEST(ThreadQueueTest, PushLogsWhenLoggerProvided) {
  auto logger = std::make_shared<NiceMock<MockLogger>>();
  EXPECT_CALL(*logger, info("キューへメッセージ追加成功"));
  EXPECT_CALL(*logger, info("キューからメッセージ取得成功"));
  ThreadQueue q(logger);
  auto msg = std::make_shared<Message>(MessageType::StartBuzzer,
                                       std::vector<std::string>{"2"}, logger);
  q.push(msg);
  auto res = q.pop();
  ASSERT_NE(res, nullptr);
  EXPECT_EQ(res->type(), msg->type());
  EXPECT_EQ(res->payload(), msg->payload());
}

TEST(ThreadQueueTest, PushMultipleMaintainsFIFO) {
  ThreadQueue q(nullptr);
  auto msg_logger = std::make_shared<NiceMock<MockLogger>>();
  auto msg1 = std::make_shared<Message>(MessageType::StartBuzzer,
                                        std::vector<std::string>{"a"}, msg_logger);
  auto msg2 = std::make_shared<Message>(MessageType::StopBuzzer,
                                        std::vector<std::string>{"b"}, msg_logger);
  q.push(msg1);
  q.push(msg2);
  auto first = q.pop();
  auto second = q.pop();
  ASSERT_NE(first, nullptr);
  ASSERT_NE(second, nullptr);
  EXPECT_EQ(first->type(), msg1->type());
  EXPECT_EQ(second->type(), msg2->type());
}

TEST(ThreadQueueTest, PushNullptrThrowsAndLogsError) {
  auto logger = std::make_shared<NiceMock<MockLogger>>();
  EXPECT_CALL(*logger, error(testing::_));
  ThreadQueue q(logger);
  EXPECT_THROW(q.push(nullptr), std::invalid_argument);
  EXPECT_EQ(q.size(), 0u);
}

