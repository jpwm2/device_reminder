#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "main_task/main_task.hpp"
#include "message/message.hpp"

using ::testing::StrictMock;
using ::testing::NiceMock;

namespace device_reminder {

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

TEST(MainTaskTest, HumanDetectedStartsScanAndTimer) {
    auto det_timer = std::make_shared<NiceMock<MockTimer>>();
    auto cd_timer = std::make_shared<NiceMock<MockTimer>>();
    auto human_sender = std::make_shared<StrictMock<MockSender>>();
    auto bt_sender = std::make_shared<NiceMock<MockSender>>();
    auto buzzer_sender = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    MainTask task(det_timer, cd_timer, human_sender, bt_sender, buzzer_sender, logger);

    EXPECT_CALL(*det_timer, start(4000, testing::Field(&Message::payload_, true)));
    EXPECT_CALL(*bt_sender, enqueue(testing::Field(&Message::type_, MessageType::StartScan)));

    task.run(Message{MessageType::HumanDetected});
    EXPECT_EQ(task.state(), MainTask::State::WaitDeviceResponse);
}

TEST(MainTaskTest, DeviceDetectedStopsTimerAndNotifies) {
    auto det_timer = std::make_shared<NiceMock<MockTimer>>();
    auto cd_timer = std::make_shared<NiceMock<MockTimer>>();
    auto human_sender = std::make_shared<StrictMock<MockSender>>();
    auto bt_sender = std::make_shared<NiceMock<MockSender>>();
    auto buzzer_sender = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    MainTask task(det_timer, cd_timer, human_sender, bt_sender, buzzer_sender, logger);

    task.run(Message{MessageType::HumanDetected});

    EXPECT_CALL(*human_sender, enqueue(testing::Field(&Message::type_, MessageType::HumanDetectStart)));
    EXPECT_CALL(*buzzer_sender, enqueue(testing::Field(&Message::type_, MessageType::BuzzerOff)));
    EXPECT_CALL(*det_timer, active()).WillOnce(testing::Return(true));
    EXPECT_CALL(*det_timer, stop());

    task.run(Message{MessageType::DeviceScanResult, true});
    EXPECT_EQ(task.state(), MainTask::State::WaitHumanDetect);
}

TEST(MainTaskTest, DeviceNotDetectedStartsCooldown) {
    auto det_timer = std::make_shared<NiceMock<MockTimer>>();
    auto cd_timer = std::make_shared<NiceMock<MockTimer>>();
    auto human_sender = std::make_shared<StrictMock<MockSender>>();
    auto bt_sender = std::make_shared<NiceMock<MockSender>>();
    auto buzzer_sender = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    MainTask task(det_timer, cd_timer, human_sender, bt_sender, buzzer_sender, logger);
    task.run(Message{MessageType::HumanDetected});

    EXPECT_CALL(*cd_timer, start(1000, testing::Field(&Message::payload_, false)));
    task.run(Message{MessageType::DeviceScanResult, false});
    EXPECT_EQ(task.state(), MainTask::State::ScanCooldown);
}

TEST(MainTaskTest, CooldownTimeoutRestartsScan) {
    auto det_timer = std::make_shared<NiceMock<MockTimer>>();
    auto cd_timer = std::make_shared<NiceMock<MockTimer>>();
    auto human_sender = std::make_shared<StrictMock<MockSender>>();
    auto bt_sender = std::make_shared<NiceMock<MockSender>>();
    auto buzzer_sender = std::make_shared<NiceMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    MainTask task(det_timer, cd_timer, human_sender, bt_sender, buzzer_sender, logger);
    task.run(Message{MessageType::HumanDetected});
    task.run(Message{MessageType::DeviceScanResult, false});

    EXPECT_CALL(*bt_sender, enqueue(testing::Field(&Message::type_, MessageType::StartScan)));
    task.run(Message{MessageType::Timeout, false});
    EXPECT_EQ(task.state(), MainTask::State::WaitDeviceResponse);
}

TEST(MainTaskTest, DetectionTimeoutReturnsToWaitHuman) {
    auto det_timer = std::make_shared<NiceMock<MockTimer>>();
    auto cd_timer = std::make_shared<NiceMock<MockTimer>>();
    auto human_sender = std::make_shared<StrictMock<MockSender>>();
    auto bt_sender = std::make_shared<NiceMock<MockSender>>();
    auto buzzer_sender = std::make_shared<NiceMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    MainTask task(det_timer, cd_timer, human_sender, bt_sender, buzzer_sender, logger);
    task.run(Message{MessageType::HumanDetected});

    task.run(Message{MessageType::Timeout, true});
    EXPECT_EQ(task.state(), MainTask::State::WaitHumanDetect);
}

} // namespace device_reminder
