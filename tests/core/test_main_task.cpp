#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "main_task/main_task.hpp"
#include "infra/thread_operation/thread_message/thread_message.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"

using ::testing::StrictMock;
using ::testing::NiceMock;

namespace device_reminder {

class MockTimer : public ITimerService {
public:
    MOCK_METHOD(void, start, (uint32_t, const ProcessMessage&), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, active, (), (const, noexcept, override));
};

class MockSender : public IProcessSender {
public:
    MOCK_METHOD(void, send, (), (override));
};

class MockFileLoader : public IFileLoader {
public:
    MOCK_METHOD(int, load_int, (), (const, override));
    MOCK_METHOD(std::string, load_string, (), (const, override));
    MOCK_METHOD(std::vector<std::string>, load_string_list, (), (const, override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
};

TEST(MainTaskTest, HumanDetectedStartsScanAndTimer) {
    auto det_timer = std::make_shared<NiceMock<MockTimer>>();
    auto cd_timer = std::make_shared<NiceMock<MockTimer>>();
    auto file_loader = std::make_shared<NiceMock<MockFileLoader>>();
    auto human_start = std::make_shared<StrictMock<MockSender>>();
    auto human_stop = std::make_shared<StrictMock<MockSender>>();
    auto bt_sender = std::make_shared<StrictMock<MockSender>>();
    auto buz_start = std::make_shared<StrictMock<MockSender>>();
    auto buz_stop = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    MainTask task(logger, file_loader, human_start, human_stop, bt_sender, buz_start, buz_stop, det_timer, cd_timer);

    EXPECT_CALL(*det_timer, start(4000, testing::_));
    EXPECT_CALL(*bt_sender, send());

    task.run(ThreadMessage{ThreadMessageType::HumanDetected, {}});
    EXPECT_EQ(task.state(), MainTask::State::WaitDeviceResponse);
}

TEST(MainTaskTest, DeviceDetectedStopsTimerAndNotifies) {
    auto det_timer = std::make_shared<NiceMock<MockTimer>>();
    auto cd_timer = std::make_shared<NiceMock<MockTimer>>();
    auto file_loader = std::make_shared<NiceMock<MockFileLoader>>();
    auto human_start = std::make_shared<StrictMock<MockSender>>();
    auto human_stop = std::make_shared<StrictMock<MockSender>>();
    auto bt_sender = std::make_shared<StrictMock<MockSender>>();
    auto buz_start = std::make_shared<StrictMock<MockSender>>();
    auto buz_stop = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    EXPECT_CALL(*file_loader, load_string_list()).WillOnce(testing::Return(std::vector<std::string>{"phone"}));

    MainTask task(logger, file_loader, human_start, human_stop, bt_sender, buz_start, buz_stop, det_timer, cd_timer);

    task.run(ThreadMessage{ThreadMessageType::HumanDetected, {}});

    EXPECT_CALL(*human_start, send());
    EXPECT_CALL(*buz_stop, send());
    EXPECT_CALL(*det_timer, active()).WillOnce(testing::Return(true));
    EXPECT_CALL(*det_timer, stop());

    task.run(ThreadMessage{ThreadMessageType::RespondDeviceFound, {"phone"}});
    EXPECT_EQ(task.state(), MainTask::State::WaitHumanDetect);
}

TEST(MainTaskTest, DeviceNotDetectedStartsCooldown) {
    auto det_timer = std::make_shared<NiceMock<MockTimer>>();
    auto cd_timer = std::make_shared<NiceMock<MockTimer>>();
    auto file_loader = std::make_shared<NiceMock<MockFileLoader>>();
    auto human_start = std::make_shared<StrictMock<MockSender>>();
    auto human_stop = std::make_shared<StrictMock<MockSender>>();
    auto bt_sender = std::make_shared<StrictMock<MockSender>>();
    auto buz_start = std::make_shared<StrictMock<MockSender>>();
    auto buz_stop = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    EXPECT_CALL(*file_loader, load_string_list()).WillOnce(testing::Return(std::vector<std::string>{"phone"}));

    MainTask task(logger, file_loader, human_start, human_stop, bt_sender, buz_start, buz_stop, det_timer, cd_timer);
    task.run(ThreadMessage{ThreadMessageType::HumanDetected, {}});

    EXPECT_CALL(*buz_start, send());
    EXPECT_CALL(*cd_timer, start(1000, testing::_));
    task.run(ThreadMessage{ThreadMessageType::RespondDeviceNotFound, {"other"}});
    EXPECT_EQ(task.state(), MainTask::State::ScanCooldown);
}

TEST(MainTaskTest, CooldownTimeoutRestartsScan) {
    auto det_timer = std::make_shared<NiceMock<MockTimer>>();
    auto cd_timer = std::make_shared<NiceMock<MockTimer>>();
    auto file_loader = std::make_shared<NiceMock<MockFileLoader>>();
    auto human_start = std::make_shared<StrictMock<MockSender>>();
    auto human_stop = std::make_shared<StrictMock<MockSender>>();
    auto bt_sender = std::make_shared<StrictMock<MockSender>>();
    auto buz_start = std::make_shared<NiceMock<MockSender>>();
    auto buz_stop = std::make_shared<NiceMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    EXPECT_CALL(*file_loader, load_string_list()).WillRepeatedly(testing::Return(std::vector<std::string>{"phone"}));

    MainTask task(logger, file_loader, human_start, human_stop, bt_sender, buz_start, buz_stop, det_timer, cd_timer);
    task.run(ThreadMessage{ThreadMessageType::HumanDetected, {}});
    task.run(ThreadMessage{ThreadMessageType::RespondDeviceNotFound, {"other"}});

    EXPECT_CALL(*bt_sender, send());
    task.run(ThreadMessage{ThreadMessageType::ProcessingTimeout, {std::to_string(static_cast<int>(MainTask::TimerId::T_COOLDOWN))}});
    EXPECT_EQ(task.state(), MainTask::State::WaitDeviceResponse);
}

TEST(MainTaskTest, DetectionTimeoutReturnsToWaitHuman) {
    auto det_timer = std::make_shared<NiceMock<MockTimer>>();
    auto cd_timer = std::make_shared<NiceMock<MockTimer>>();
    auto file_loader = std::make_shared<NiceMock<MockFileLoader>>();
    auto human_start = std::make_shared<StrictMock<MockSender>>();
    auto human_stop = std::make_shared<StrictMock<MockSender>>();
    auto bt_sender = std::make_shared<StrictMock<MockSender>>();
    auto buz_start = std::make_shared<NiceMock<MockSender>>();
    auto buz_stop = std::make_shared<NiceMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    MainTask task(logger, file_loader, human_start, human_stop, bt_sender, buz_start, buz_stop, det_timer, cd_timer);
    task.run(ThreadMessage{ThreadMessageType::HumanDetected, {}});

    task.run(ThreadMessage{ThreadMessageType::ProcessingTimeout, {std::to_string(static_cast<int>(MainTask::TimerId::T_DET_TIMEOUT))}});
    EXPECT_EQ(task.state(), MainTask::State::WaitHumanDetect);
}

} // namespace device_reminder
