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

} // namespace device_reminder

