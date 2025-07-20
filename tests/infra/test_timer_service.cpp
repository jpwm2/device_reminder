#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/timer_service/timer_service.hpp"
#include "infra/message_operator/i_message_sender.hpp"
#include <thread>
#include <chrono>

using namespace device_reminder;
using ::testing::StrictMock;
using ::testing::NiceMock;

namespace {
class MockSender : public IMessageSender {
public:
    MOCK_METHOD(bool, enqueue, (const Message&), (override));
    MOCK_METHOD(void, stop, (), (override));
};
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
};
} // namespace

TEST(TimerServiceTest, SendsTimeoutMessage) {
    auto sender = std::make_shared<StrictMock<MockSender>>();
    NiceMock<MockLogger> logger;
    TimerService timer(sender, std::shared_ptr<ILogger>(&logger, [](ILogger*){}));
    Message m{MessageType::Timeout, false};
    EXPECT_CALL(*sender, enqueue(testing::Field(&Message::type_, MessageType::Timeout))).Times(1);
    timer.start(10, m);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    while (timer.active()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    timer.stop();
}
