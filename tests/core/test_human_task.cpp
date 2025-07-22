#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "human_task/human_task.hpp"
#include "thread_message_operation/thread_message.hpp"
#include "process_message_operation/i_process_message_sender.hpp"

using ::testing::StrictMock;
using ::testing::NiceMock;

namespace device_reminder {

class MockPIR : public IPIRDriver {
public:
    MOCK_METHOD(int, read, (), (override));
};

class MockTimer : public ITimerService {
public:
    MOCK_METHOD(void, start, (uint32_t, const ProcessMessage&), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, active, (), (const, noexcept, override));
};

class MockSender : public IProcessMessageSender {
public:
    MOCK_METHOD(bool, enqueue, (const ProcessMessage&), (override));
    MOCK_METHOD(void, stop, (), (override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
};

TEST(HumanTaskTest, StopRequestTransitionsToStopped) {
    auto pir = std::make_shared<NiceMock<MockPIR>>();
    auto timer = std::make_shared<NiceMock<MockTimer>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    HumanTask task(pir, timer, sender, logger);
    EXPECT_EQ(task.state(), HumanTask::State::Detecting);

    task.run(ThreadMessage{MessageType::HumanDetectStop});
    EXPECT_EQ(task.state(), HumanTask::State::Stopped);
}

TEST(HumanTaskTest, StartRequestStartsCooldown) {
    auto pir = std::make_shared<NiceMock<MockPIR>>();
    auto timer = std::make_shared<NiceMock<MockTimer>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    HumanTask task(pir, timer, sender, logger);
    task.run(ThreadMessage{MessageType::HumanDetectStop});

    EXPECT_CALL(*timer, start(testing::_, testing::_));
    task.run(ThreadMessage{MessageType::HumanDetectStart});
    EXPECT_EQ(task.state(), HumanTask::State::Cooldown);
}

TEST(HumanTaskTest, TimeoutReturnsToDetecting) {
    auto pir = std::make_shared<NiceMock<MockPIR>>();
    auto timer = std::make_shared<NiceMock<MockTimer>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    HumanTask task(pir, timer, sender, logger);
    task.run(ThreadMessage{MessageType::HumanDetectStop});
    task.run(ThreadMessage{MessageType::HumanDetectStart});
    EXPECT_EQ(task.state(), HumanTask::State::Cooldown);

    task.run(ThreadMessage{MessageType::Timeout});
    EXPECT_EQ(task.state(), HumanTask::State::Detecting);
}

TEST(HumanTaskTest, HumanDetectedSendsMessage) {
    auto pir = std::make_shared<NiceMock<MockPIR>>();
    auto timer = std::make_shared<NiceMock<MockTimer>>();
    auto sender = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    HumanTask task(pir, timer, sender, logger);

    EXPECT_CALL(*sender, enqueue(testing::Field(&ProcessMessage::type_, MessageType::HumanDetected)));
    task.run(ThreadMessage{MessageType::HumanDetected});
}

} // namespace device_reminder
