#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "infra/logger/logger.hpp"
#include "infra/thread_operation/thread_message/thread_message.hpp"
#include "infra/thread_operation/thread_queue/thread_queue.hpp"

#include <spdlog/logger.h>
#include <spdlog/sinks/sink.h>

using ::testing::StrictMock;
using ::testing::Truly;

namespace {
class MockSink : public spdlog::sinks::sink {
public:
  MOCK_METHOD(void, log, (const spdlog::details::log_msg &msg), (override));
  MOCK_METHOD(void, flush, (), (override));
  MOCK_METHOD(void, set_pattern, (const std::string &pattern), (override));
  void set_formatter(std::unique_ptr<spdlog::formatter>) override {}
};
} // namespace

using namespace device_reminder;

TEST(ThreadQueueIntegrationTest, PushAndPop) {
  auto sink = std::make_shared<StrictMock<MockSink>>();
  auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
  auto logger = std::make_shared<Logger>(spdlogger);
  ThreadQueue queue(logger);

  auto msg = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing,
                                             std::vector<std::string>{"1"});

  EXPECT_CALL(*sink, log(Truly([](const spdlog::details::log_msg &m) {
    return m.level == spdlog::level::info && m.payload == "ThreadQueue push";
  })));

  queue.push(msg);
  auto popped = queue.pop();

  ASSERT_EQ(popped, msg);
  EXPECT_EQ(queue.size(), 0u);
}

TEST(ThreadQueueIntegrationTest, PushNullMessage) {
  auto sink = std::make_shared<StrictMock<MockSink>>();
  auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
  auto logger = std::make_shared<Logger>(spdlogger);
  ThreadQueue queue(logger);

  EXPECT_CALL(*sink, log(Truly([](const spdlog::details::log_msg &m) {
    return m.level == spdlog::level::info && m.payload == "ThreadQueue push";
  })));

  queue.push(nullptr);
  auto popped = queue.pop();

  EXPECT_EQ(popped, nullptr);
  EXPECT_EQ(queue.size(), 0u);
}

TEST(ThreadQueueIntegrationTest, PopEmptyQueue) {
  auto sink = std::make_shared<StrictMock<MockSink>>();
  auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
  auto logger = std::make_shared<Logger>(spdlogger);
  ThreadQueue queue(logger);

  EXPECT_CALL(*sink, log).Times(0);

  auto popped = queue.pop();

  EXPECT_EQ(popped, nullptr);
  EXPECT_EQ(queue.size(), 0u);
}
