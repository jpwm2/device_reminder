#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/timer_service/timer_service.hpp"
#include "infra/thread_operation/thread_sender/i_thread_sender.hpp"
#include "infra/thread_operation/thread_message/thread_message.hpp"
#include <thread>
#include <chrono>

using namespace device_reminder;
using ::testing::StrictMock;
using ::testing::NiceMock;

namespace {
class MockSender : public IThreadSender {
public:
    MOCK_METHOD(void, send, (), (override));
};
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};
} // namespace

TEST(TimerServiceTest, SendsTimeoutMessage) {
    auto sender = std::make_shared<StrictMock<MockSender>>();
    NiceMock<MockLogger> logger;
    TimerService timer(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), 10, sender);
    EXPECT_CALL(*sender, send()).Times(1);
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    timer.stop();
}
