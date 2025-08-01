#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/timer_service/timer_service.hpp"
#include "infra/thread_operation/thread_sender/i_thread_sender.hpp"
#include "infra/logger/i_logger.hpp"

#include <chrono>
#include <thread>

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

TEST(TimerServiceTest, ConstructorLogsCreationWithValidArgs) {
    StrictMock<MockLogger> logger;
    auto sender = std::make_shared<StrictMock<MockSender>>();

    EXPECT_CALL(logger, info("TimerService created")).Times(1);
    {
        TimerService service(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), 10, sender);
    }
}

TEST(TimerServiceTest, ConstructorNoLogWhenLoggerNull) {
    auto sender = std::make_shared<StrictMock<MockSender>>();

    TimerService service(nullptr, 10, sender);
    // Expect no crash and no log; using NiceMock to ignore
}
TEST(TimerServiceTest, ConstructorHandlesNegativeDuration) {
    NiceMock<MockLogger> logger;
    auto sender = std::make_shared<StrictMock<MockSender>>();
    TimerService service(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), -5, sender);
    (void)service;
}


TEST(TimerServiceTest, StartSendsMessageAndLogs) {
    StrictMock<MockLogger> logger;
    auto sender = std::make_shared<StrictMock<MockSender>>();
    TimerService service(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), 10, sender);

    {
        ::testing::InSequence seq;
        EXPECT_CALL(logger, info("TimerService started")).Times(1);
        EXPECT_CALL(*sender, send()).Times(1);
        EXPECT_CALL(logger, info("TimerService timeout")).Times(1);
        EXPECT_CALL(logger, info("TimerService stopped")).Times(1);
    }

    service.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    service.stop();
}

TEST(TimerServiceTest, StartWithoutSenderOnlyLogsStart) {
    StrictMock<MockLogger> logger;
    TimerService service(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), 10, nullptr);

    EXPECT_CALL(logger, info("TimerService started")).Times(1);
    EXPECT_CALL(logger, info("TimerService timeout")).Times(0);
    EXPECT_CALL(logger, info("TimerService stopped")).Times(1);

    service.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    service.stop();
}

TEST(TimerServiceTest, StartWithNullLoggerStillSends) {
    auto sender = std::make_shared<StrictMock<MockSender>>();
    TimerService service(nullptr, 10, sender);

    EXPECT_CALL(*sender, send()).Times(1);

    service.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    service.stop();
}


TEST(TimerServiceTest, StopWithoutStartLogsOnce) {
    StrictMock<MockLogger> logger;
    auto sender = std::make_shared<StrictMock<MockSender>>();
    TimerService service(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), 10, sender);

    EXPECT_CALL(logger, info("TimerService stopped")).Times(1);
    service.stop();
}


