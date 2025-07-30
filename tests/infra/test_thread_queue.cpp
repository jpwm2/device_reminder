#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "infra/logger/i_logger.hpp"
#include "infra/thread_operation/thread_message/thread_message.hpp"
#include "infra/thread_operation/thread_queue/thread_queue.hpp"

using namespace device_reminder;
using ::testing::NiceMock;

namespace {
class MockLogger : public ILogger {
public:
  MOCK_METHOD(void, info, (const std::string &), (override));
  MOCK_METHOD(void, error, (const std::string &), (override));
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
