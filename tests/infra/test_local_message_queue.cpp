#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/message_operator/local_message_queue.hpp"
#include "infra/logger/i_logger.hpp"

using namespace device_reminder;

namespace {
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
};
} // namespace

TEST(LocalMessageQueueTest, PushPopWorks) {
    LocalMessageQueue q;
    Message m{MessageType::BuzzerOn, true};
    EXPECT_TRUE(q.push(m));
    auto res = q.pop();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->type_, m.type_);
    EXPECT_EQ(res->payload_, m.payload_);
}

TEST(LocalMessageQueueTest, CloseMakesPopReturnFalse) {
    LocalMessageQueue q;
    q.close();
    EXPECT_FALSE(q.is_open());
    Message out{};
    EXPECT_FALSE(q.pop(out));
}
