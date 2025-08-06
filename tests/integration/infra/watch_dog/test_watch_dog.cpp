#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/watch_dog/watch_dog.hpp"
#include "infra/timer_service/timer_service.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/thread_operation/thread_sender/i_thread_sender.hpp"

#include <chrono>
#include <thread>

using ::testing::StrictMock;
using ::testing::InSequence;

namespace device_reminder {

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string& msg), (override));
    MOCK_METHOD(void, error, (const std::string& msg), (override));
    MOCK_METHOD(void, warn, (const std::string& msg), (override));
};

class MockThreadSender : public IThreadSender {
public:
    MOCK_METHOD(void, send, (), (override));
};

} // namespace device_reminder

TEST(WatchDogIntegrationTest, StartTriggersTimeoutAndSend) {
    using namespace device_reminder;

    StrictMock<MockLogger> logger;
    StrictMock<MockThreadSender> sender;

    {
        InSequence seq;
        EXPECT_CALL(logger, info("TimerService created"));
        EXPECT_CALL(logger, info("WDT監視開始"));
        EXPECT_CALL(logger, info("TimerService stopped"));
        EXPECT_CALL(logger, info("TimerService started"));
        EXPECT_CALL(logger, info("TimerService send succeeded"));
        EXPECT_CALL(logger, info("TimerService stopped"));
        EXPECT_CALL(logger, info("TimerService destroyed"));
    }
    EXPECT_CALL(sender, send()).Times(1);

    auto logger_ptr = std::shared_ptr<ILogger>(&logger, [](ILogger*){});
    auto sender_ptr = std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){});

    auto timer = std::make_shared<TimerService>(logger_ptr, 10, sender_ptr);

    {
        WatchDog wd(timer, logger_ptr);
        wd.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    timer.reset();
}

TEST(WatchDogIntegrationTest, StopBeforeTimeoutDoesNotSend) {
    using namespace device_reminder;

    StrictMock<MockLogger> logger;
    StrictMock<MockThreadSender> sender;

    {
        InSequence seq;
        EXPECT_CALL(logger, info("TimerService created"));
        EXPECT_CALL(logger, info("WDT監視開始"));
        EXPECT_CALL(logger, info("TimerService stopped"));
        EXPECT_CALL(logger, info("TimerService started"));
        EXPECT_CALL(logger, info("WDT監視停止"));
        EXPECT_CALL(logger, info("TimerService stopped"));
        EXPECT_CALL(logger, info("TimerService stopped"));
        EXPECT_CALL(logger, info("TimerService destroyed"));
    }
    EXPECT_CALL(sender, send()).Times(0);

    auto logger_ptr = std::shared_ptr<ILogger>(&logger, [](ILogger*){});
    auto sender_ptr = std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){});

    auto timer = std::make_shared<TimerService>(logger_ptr, 50, sender_ptr);

    {
        WatchDog wd(timer, logger_ptr);
        wd.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        wd.stop();
    }
    timer.reset();
}

