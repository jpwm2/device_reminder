#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "buzzer_task/buzzer_task.hpp"
#include "thread_message_operation/thread_message.hpp"

using ::testing::StrictMock;
using ::testing::NiceMock;

namespace device_reminder {

class MockDriver : public IBuzzerDriver {
public:
    MOCK_METHOD(bool, start, (), (override));
    MOCK_METHOD(bool, stop, (), (override));
};

class MockTimer : public ITimerService {
public:
    MOCK_METHOD(void, start, (uint32_t, const ProcessMessage&), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, active, (), (const, noexcept, override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
};

TEST(BuzzerTaskTest, StartAndTimeoutStopsBuzzer) {
    auto driver = std::make_shared<StrictMock<MockDriver>>();
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    BuzzerTask task(driver, timer, logger);

    EXPECT_CALL(*driver, start());
    EXPECT_CALL(*timer, start(4000, testing::Field(&ProcessMessage::type_, ThreadMessageType::Timeout)));
    task.send_message(ThreadMessage{ThreadMessageType::StartBuzzer});
    EXPECT_EQ(task.state(), BuzzerTask::State::Buzzing);

    EXPECT_CALL(*driver, stop());
    EXPECT_CALL(*timer, stop()).Times(0);
    task.send_message(ThreadMessage{ThreadMessageType::Timeout});
    EXPECT_EQ(task.state(), BuzzerTask::State::WaitStart);
}

TEST(BuzzerTaskTest, ManualStopCancelsTimer) {
    auto driver = std::make_shared<StrictMock<MockDriver>>();
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    BuzzerTask task(driver, timer, logger);

    EXPECT_CALL(*driver, start());
    EXPECT_CALL(*timer, start(4000, testing::Field(&ProcessMessage::type_, ThreadMessageType::Timeout)));
    task.send_message(ThreadMessage{ThreadMessageType::StartBuzzer});

    EXPECT_CALL(*driver, stop());
    EXPECT_CALL(*timer, stop());
    task.send_message(ThreadMessage{ThreadMessageType::StopBuzzer});
    EXPECT_EQ(task.state(), BuzzerTask::State::WaitStart);
}

TEST(BuzzerTaskTest, IgnoreDuplicateStart) {
    auto driver = std::make_shared<StrictMock<MockDriver>>();
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    BuzzerTask task(driver, timer, logger);

    EXPECT_CALL(*driver, start());
    EXPECT_CALL(*timer, start(4000, testing::Field(&ProcessMessage::type_, ThreadMessageType::Timeout)));
    task.send_message(ThreadMessage{ThreadMessageType::StartBuzzer});
    EXPECT_EQ(task.state(), BuzzerTask::State::Buzzing);

    task.send_message(ThreadMessage{ThreadMessageType::StartBuzzer});
    EXPECT_EQ(task.state(), BuzzerTask::State::Buzzing);
}

} // namespace device_reminder
