#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/timer_service/timer_service.hpp"
#include "infra/thread_operation/thread_sender/i_thread_sender.hpp"
#include "infra/logger/i_logger.hpp"

#include <chrono>
#include <thread>

using namespace device_reminder;
using ::testing::NiceMock;
using ::testing::StrictMock;

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

TEST(TimerServiceTest, ConstructorAndDestructorWithLogger) {
    StrictMock<MockSender> sender;
    StrictMock<MockLogger> logger;
    {
        testing::InSequence seq;
        EXPECT_CALL(logger, info("TimerService created"));
        EXPECT_CALL(logger, info("TimerService stopped"));
        EXPECT_CALL(logger, info("TimerService destroyed"));
    }
    {
        TimerService timer(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                           10,
                           std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}));
    }
}

TEST(TimerServiceTest, StartTriggersSendAndTimeoutLog) {
    StrictMock<MockSender> sender;
    StrictMock<MockLogger> logger;
    {
        testing::InSequence seq;
        EXPECT_CALL(logger, info("TimerService created"));
        EXPECT_CALL(logger, info("TimerService stopped"));
        EXPECT_CALL(logger, info("TimerService started"));
        EXPECT_CALL(sender, send()).Times(1);
        EXPECT_CALL(logger, info("TimerService timeout"));
        EXPECT_CALL(logger, info("TimerService stopped"));
        EXPECT_CALL(logger, info("TimerService destroyed"));
    }
    {
        TimerService timer(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                           10,
                           std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}));
        timer.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

TEST(TimerServiceTest, StartWithNullSenderDoesNotSend) {
    NiceMock<MockLogger> logger;
    {
        testing::InSequence seq;
        EXPECT_CALL(logger, info("TimerService created"));
        EXPECT_CALL(logger, info("TimerService stopped"));
        EXPECT_CALL(logger, info("TimerService started"));
        EXPECT_CALL(logger, info("TimerService timeout")).Times(0);
        EXPECT_CALL(logger, info("TimerService stopped"));
        EXPECT_CALL(logger, info("TimerService destroyed"));
    }
    {
        TimerService timer(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), 10, nullptr);
        timer.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

TEST(TimerServiceTest, StartWithoutLoggerStillSends) {
    StrictMock<MockSender> sender;
    {
        EXPECT_CALL(sender, send()).Times(1);
    }
    {
        TimerService timer(nullptr, 5,
                           std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}));
        timer.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

TEST(TimerServiceTest, StopBeforeTimeoutPreventsSend) {
    StrictMock<MockSender> sender;
    NiceMock<MockLogger> logger;
    {
        testing::InSequence seq;
        EXPECT_CALL(logger, info("TimerService created"));
        EXPECT_CALL(logger, info("TimerService stopped"));
        EXPECT_CALL(logger, info("TimerService started"));
        EXPECT_CALL(sender, send()).Times(0);
        EXPECT_CALL(logger, info("TimerService stopped"));
        EXPECT_CALL(logger, info("TimerService stopped"));
        EXPECT_CALL(logger, info("TimerService destroyed"));
    }
    {
        TimerService timer(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                           50,
                           std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}));
        timer.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // stop before the 50ms timeout triggers
        timer.stop();
    }
}

TEST(TimerServiceTest, ZeroDurationTimeoutTriggersImmediately) {
    StrictMock<MockSender> sender;
    NiceMock<MockLogger> logger;
    {
        testing::InSequence seq;
        EXPECT_CALL(logger, info("TimerService created"));
        EXPECT_CALL(logger, info("TimerService stopped"));
        EXPECT_CALL(logger, info("TimerService started"));
        EXPECT_CALL(sender, send()).Times(1);
        EXPECT_CALL(logger, info("TimerService timeout"));
        EXPECT_CALL(logger, info("TimerService stopped"));
        EXPECT_CALL(logger, info("TimerService destroyed"));
    }
    {
        TimerService timer(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                           0,
                           std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}));
        timer.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}
