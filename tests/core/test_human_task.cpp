#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "human_task/human_task.hpp"
#include "message/message.hpp"

using ::testing::StrictMock;
using ::testing::NiceMock;

namespace device_reminder {

class MockPIR : public IPIRDriver {
public:
    MOCK_METHOD(int, read, (), (override));
};

class MockTimer : public ITimerService {
public:
    MOCK_METHOD(void, start, (uint32_t, const Message&), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, active, (), (const, noexcept, override));
};

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

TEST(HumanTaskTest, StopRequestTransitionsToStopped) {
    auto pir = std::make_shared<NiceMock<MockPIR>>();
    auto timer = std::make_shared<NiceMock<MockTimer>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    HumanTask task(pir, timer, sender, logger);
    EXPECT_EQ(task.state(), HumanTask::State::Detecting);

    task.run(Message{MessageType::HumanDetectStop});
    EXPECT_EQ(task.state(), HumanTask::State::Stopped);
}

TEST(HumanTaskTest, StartRequestStartsCooldown) {
    auto pir = std::make_shared<NiceMock<MockPIR>>();
    auto timer = std::make_shared<NiceMock<MockTimer>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    HumanTask task(pir, timer, sender, logger);
    task.run(Message{MessageType::HumanDetectStop});

    EXPECT_CALL(*timer, start(testing::_, testing::_));
    task.run(Message{MessageType::HumanDetectStart});
    EXPECT_EQ(task.state(), HumanTask::State::Cooldown);
}

TEST(HumanTaskTest, TimeoutReturnsToDetecting) {
    auto pir = std::make_shared<NiceMock<MockPIR>>();
    auto timer = std::make_shared<NiceMock<MockTimer>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    HumanTask task(pir, timer, sender, logger);
    task.run(Message{MessageType::HumanDetectStop});
    task.run(Message{MessageType::HumanDetectStart});
    EXPECT_EQ(task.state(), HumanTask::State::Cooldown);

    task.run(Message{MessageType::Timeout});
    EXPECT_EQ(task.state(), HumanTask::State::Detecting);
}

TEST(HumanTaskTest, HumanDetectedSendsMessage) {
    auto pir = std::make_shared<NiceMock<MockPIR>>();
    auto timer = std::make_shared<NiceMock<MockTimer>>();
    auto sender = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    HumanTask task(pir, timer, sender, logger);

    EXPECT_CALL(*sender, enqueue(testing::Field(&Message::type_, MessageType::HumanDetected)));
    task.run(Message{MessageType::HumanDetected});
}

} // namespace device_reminder
