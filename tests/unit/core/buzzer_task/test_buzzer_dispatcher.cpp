#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "buzzer_task/buzzer_handler.hpp"
#include "infra/process_operation/process_message/process_message.hpp"

using ::testing::StrictMock;
using ::testing::NiceMock;

namespace device_reminder {

class MockBuzzerTask : public IBuzzerTask {
public:
    void run() override {}
    bool send_message(const IThreadMessage&) override { return true; }
    MOCK_METHOD(void, on_waiting, (const std::vector<std::string>&), (override));
    MOCK_METHOD(void, on_buzzing, (const std::vector<std::string>&), (override));
};

class MockTimer : public ITimerService {
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};

class DummyLogger : public ILogger {
public:
    void info(const std::string&) override {}
    void error(const std::string&) override {}
    void warn(const std::string&) override {}
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

TEST(BuzzerHandlerTest, StartBuzzingStartsTimerAndCallsTask) {
    auto task = std::make_shared<StrictMock<MockBuzzerTask>>();
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    auto logger = std::make_shared<DummyLogger>();
    BuzzerHandler handler(logger, task, timer);

    EXPECT_CALL(*task, on_buzzing(testing::_)).Times(1);
    EXPECT_CALL(*timer, start()).Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{});
    handler.handle(msg);
}

TEST(BuzzerHandlerTest, StopBuzzingStopsTimerAndCallsTask) {
    auto task = std::make_shared<StrictMock<MockBuzzerTask>>();
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    auto logger = std::make_shared<DummyLogger>();
    BuzzerHandler handler(logger, task, timer);

    EXPECT_CALL(*task, on_waiting(testing::_)).Times(1);
    EXPECT_CALL(*timer, stop()).Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StopBuzzing, std::vector<std::string>{});
    handler.handle(msg);
}

TEST(BuzzerHandlerTest, TimeoutStopsTimerAndCallsTask) {
    auto task = std::make_shared<StrictMock<MockBuzzerTask>>();
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    auto logger = std::make_shared<DummyLogger>();
    BuzzerHandler handler(logger, task, timer);

    EXPECT_CALL(*task, on_waiting(testing::_)).Times(1);
    EXPECT_CALL(*timer, stop()).Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::BuzzTimeout, std::vector<std::string>{});
    handler.handle(msg);
}

TEST(BuzzerHandlerTest, ConstructorLogsCreation) {
    StrictMock<MockLogger> logger;
    auto task = std::make_shared<NiceMock<MockBuzzerTask>>();
    auto timer = std::make_shared<NiceMock<MockTimer>>();
    EXPECT_CALL(logger, info("BuzzerHandler created")).Times(1);
    BuzzerHandler handler(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), task, timer);
}

TEST(BuzzerHandlerTest, HandleNullMessageDoesNothing) {
    StrictMock<MockLogger> logger;
    StrictMock<MockBuzzerTask> task;
    StrictMock<MockTimer> timer;
    BuzzerHandler handler(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                          std::shared_ptr<IBuzzerTask>(&task, [](IBuzzerTask*){}),
                          std::shared_ptr<ITimerService>(&timer, [](ITimerService*){}));

    EXPECT_CALL(timer, start()).Times(0);
    EXPECT_CALL(timer, stop()).Times(0);
    EXPECT_CALL(task, on_buzzing(testing::_)).Times(0);
    EXPECT_CALL(task, on_waiting(testing::_)).Times(0);
    EXPECT_CALL(logger, info(testing::_)).Times(1); // constructor only

    handler.handle(nullptr);
}

TEST(BuzzerHandlerTest, StartBuzzingWithoutTimer) {
    StrictMock<MockLogger> logger;
    StrictMock<MockBuzzerTask> task;
    BuzzerHandler handler(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                          std::shared_ptr<IBuzzerTask>(&task, [](IBuzzerTask*){}),
                          nullptr);

    {
        testing::InSequence seq;
        EXPECT_CALL(logger, info("BuzzerHandler created")).Times(1);
        EXPECT_CALL(task, on_buzzing(testing::_)).Times(1);
        EXPECT_CALL(logger, info("StartBuzzing")).Times(1);
    }

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{});
    handler.handle(msg);
}

TEST(BuzzerHandlerTest, StartBuzzingLogsMessage) {
    StrictMock<MockLogger> logger;
    StrictMock<MockBuzzerTask> task;
    StrictMock<MockTimer> timer;
    {
        testing::InSequence seq;
        EXPECT_CALL(logger, info("BuzzerHandler created")).Times(1);
        EXPECT_CALL(timer, start()).Times(1);
        EXPECT_CALL(task, on_buzzing(testing::_)).Times(1);
        EXPECT_CALL(logger, info("StartBuzzing")).Times(1);
    }

    BuzzerHandler handler(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                          std::shared_ptr<IBuzzerTask>(&task, [](IBuzzerTask*){}),
                          std::shared_ptr<ITimerService>(&timer, [](ITimerService*){}));

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{});
    handler.handle(msg);
}


} // namespace device_reminder

