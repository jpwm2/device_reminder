#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "thread_message_operation/thread_message_queue.hpp"
#include "infra/logger/i_logger.hpp"

using namespace device_reminder;

namespace {
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
};
} // namespace

TEST(ThreadMessageQueueTest, PushPopWorks) {
    ThreadMessageQueue q;
    ThreadMessage m{ThreadMessageType::StartBuzzer, true};
    EXPECT_TRUE(q.push(m));
    auto res = q.pop();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->type_, m.type_);
    EXPECT_EQ(res->payload_, m.payload_);
}

TEST(ThreadMessageQueueTest, CloseMakesPopReturnFalse) {
    ThreadMessageQueue q;
    q.close();
    EXPECT_FALSE(q.is_open());
    ThreadMessage out{};
    EXPECT_FALSE(q.pop(out));
}
