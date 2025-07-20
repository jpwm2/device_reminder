#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "bluetooth_task/bluetooth_task.hpp"
#include "infra/bluetooth_driver/i_bluetooth_driver.hpp"
#include "infra/message_operator/i_message_sender.hpp"

using ::testing::StrictMock;
using ::testing::NiceMock;
using ::testing::Return;

namespace device_reminder {

class MockDriver : public IBluetoothDriver {
public:
    MOCK_METHOD(std::vector<DeviceInfo>, scan_once, (double), (override));
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

TEST(BluetoothTaskTest, SendsDetectedTrueWhenDeviceFound) {
    auto driver = std::make_shared<StrictMock<MockDriver>>();
    auto sender = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    BluetoothTask task(driver, sender, logger);

    std::vector<DeviceInfo> devs{{"AA", -40, 1.0}};
    EXPECT_CALL(*driver, scan_once(2.0)).WillOnce(Return(devs));
    EXPECT_CALL(*sender, enqueue(testing::AllOf(
        testing::Field(&Message::type_, MessageType::DevicePresenceResponse),
        testing::Field(&Message::payload_, true))));

    task.run(Message{MessageType::BluetoothScanRequest});
    EXPECT_EQ(task.state(), BluetoothTask::State::WaitRequest);
}

TEST(BluetoothTaskTest, SendsDetectedFalseWhenNoDevice) {
    auto driver = std::make_shared<StrictMock<MockDriver>>();
    auto sender = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    BluetoothTask task(driver, sender, logger);

    EXPECT_CALL(*driver, scan_once(2.0)).WillOnce(Return(std::vector<DeviceInfo>{}));
    EXPECT_CALL(*sender, enqueue(testing::AllOf(
        testing::Field(&Message::type_, MessageType::DevicePresenceResponse),
        testing::Field(&Message::payload_, false))));

    task.run(Message{MessageType::BluetoothScanRequest});
    EXPECT_EQ(task.state(), BluetoothTask::State::WaitRequest);
}

TEST(BluetoothTaskTest, DriverErrorLogsAndSendsFalse) {
    auto driver = std::make_shared<StrictMock<MockDriver>>();
    auto sender = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    BluetoothTask task(driver, sender, logger);

    EXPECT_CALL(*driver, scan_once(2.0)).WillOnce(testing::Throw(BluetoothDriverError("fail")));
    EXPECT_CALL(*sender, enqueue(testing::AllOf(
        testing::Field(&Message::type_, MessageType::DevicePresenceResponse),
        testing::Field(&Message::payload_, false))));
    EXPECT_CALL(*logger, error(testing::HasSubstr("fail")));

    task.run(Message{MessageType::BluetoothScanRequest});
}

} // namespace device_reminder
