#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/thread_operation/thread_queue/thread_queue.hpp"
#include "infra/thread_operation/thread_message/thread_message.hpp"
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
    ThreadQueue q(nullptr);
    auto m = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzer, std::vector<std::string>{});
    q.push(m);
    auto res = q.pop();
    ASSERT_NE(res, nullptr);
    EXPECT_EQ(res->type(), m->type());
    EXPECT_EQ(res->payload(), m->payload());
}

TEST(ThreadMessageQueueTest, SizeReflectsQueue) {
    ThreadQueue q(nullptr);
    EXPECT_EQ(q.size(), 0u);
    q.push(std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzer, std::vector<std::string>{}));
    EXPECT_EQ(q.size(), 1u);
}
