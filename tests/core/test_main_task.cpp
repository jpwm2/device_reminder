#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "main_task/main_task.hpp"
#include "thread_message_operation/thread_message.hpp"
#include "process_message_operation/i_process_message_sender.hpp"

using ::testing::StrictMock;
using ::testing::NiceMock;

namespace device_reminder {

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

TEST(MainTaskTest, HumanDetectedStartsScanAndTimer) {
    auto det_timer = std::make_shared<NiceMock<MockTimer>>();
    auto cd_timer = std::make_shared<NiceMock<MockTimer>>();
    auto human_sender = std::make_shared<StrictMock<MockSender>>();
    auto bt_sender = std::make_shared<NiceMock<MockSender>>();
    auto buzzer_sender = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    MainTask task(det_timer, cd_timer, human_sender, bt_sender, buzzer_sender, logger);

    EXPECT_CALL(*det_timer, start(4000, testing::Field(&ProcessMessage::payload_, true)));
    EXPECT_CALL(*bt_sender, enqueue(testing::Field(&ProcessMessage::type_, ThreadMessageType::BluetoothScanRequested)));

    task.run(ThreadMessage{ThreadMessageType::HumanDetected});
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

    task.run(ThreadMessage{ThreadMessageType::HumanDetected});

    EXPECT_CALL(*human_sender, enqueue(testing::Field(&ProcessMessage::type_, ThreadMessageType::HumanDetectStart)));
    EXPECT_CALL(*buzzer_sender, enqueue(testing::Field(&ProcessMessage::type_, ThreadMessageType::StopBuzzer)));
    EXPECT_CALL(*det_timer, active()).WillOnce(testing::Return(true));
    EXPECT_CALL(*det_timer, stop());

    task.run(ThreadMessage{ThreadMessageType::BluetoothScanResponse, true});
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
    task.run(ThreadMessage{ThreadMessageType::HumanDetected});

    EXPECT_CALL(*cd_timer, start(1000, testing::Field(&ProcessMessage::payload_, false)));
    task.run(ThreadMessage{ThreadMessageType::BluetoothScanResponse, false});
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
    task.run(ThreadMessage{ThreadMessageType::HumanDetected});
    task.run(ThreadMessage{ThreadMessageType::BluetoothScanResponse, false});

    EXPECT_CALL(*bt_sender, enqueue(testing::Field(&ProcessMessage::type_, ThreadMessageType::BluetoothScanRequested)));
    task.run(ThreadMessage{ThreadMessageType::Timeout, false});
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
    task.run(ThreadMessage{ThreadMessageType::HumanDetected});

    task.run(ThreadMessage{ThreadMessageType::Timeout, true});
    EXPECT_EQ(task.state(), MainTask::State::WaitHumanDetect);
}

} // namespace device_reminder
