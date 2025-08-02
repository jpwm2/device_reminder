#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "core/bluetooth_task/bluetooth_handler.hpp"
#include "core/bluetooth_task/bluetooth_task.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/bluetooth_driver/i_bluetooth_driver.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"

using ::testing::StrictMock;
using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;

namespace device_reminder {

class MockProcessSender : public IProcessSender {
public:
    MOCK_METHOD(void, send, (), (override));
};

class MockFileLoader : public IFileLoader {
public:
    MOCK_METHOD(int, load_int, (const std::string&), (const, override));
    MOCK_METHOD(std::string, load_string, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<std::string>, load_string_list, (const std::string&), (const, override));
};

class MockBluetoothDriver : public IBluetoothDriver {
public:
    MOCK_METHOD(std::vector<std::string>, scan, (), (override));
};

class DummyLogger : public ILogger {
public:
    void info(const std::string&) override {}
    void error(const std::string&) override {}
    void warn(const std::string&) override {}
};

TEST(BluetoothHandlerIntegrationTest, RequestBluetoothScanTriggersSendWhenDeviceMatches) {
    auto sender = std::make_shared<StrictMock<MockProcessSender>>();
    auto loader = std::make_shared<StrictMock<MockFileLoader>>();
    auto driver = std::make_shared<StrictMock<MockBluetoothDriver>>();
    auto logger = std::make_shared<DummyLogger>();
    auto task = std::make_shared<BluetoothTask>(logger, sender, loader, driver);
    BluetoothHandler handler(logger, task);

    {
        InSequence seq;
        EXPECT_CALL(*driver, scan())
            .WillOnce(Return(std::vector<std::string>{"RegisteredDevice"}));
        EXPECT_CALL(*loader, load_string_list("device_list"))
            .WillOnce(Return(std::vector<std::string>{"RegisteredDevice"}));
        EXPECT_CALL(*sender, send()).Times(1);
    }
    EXPECT_CALL(*loader, load_int(_)).Times(0);
    EXPECT_CALL(*loader, load_string(_)).Times(0);

    auto msg = std::make_shared<ProcessMessage>(
        ProcessMessageType::RequestBluetoothScan, std::vector<std::string>{});
    handler.handle(msg);

    EXPECT_EQ(task->state(), BluetoothTask::State::WaitRequest);
}

TEST(BluetoothHandlerIntegrationTest, NullMessageDoesNothing) {
    auto sender = std::make_shared<StrictMock<MockProcessSender>>();
    auto loader = std::make_shared<StrictMock<MockFileLoader>>();
    auto driver = std::make_shared<StrictMock<MockBluetoothDriver>>();
    auto logger = std::make_shared<DummyLogger>();
    auto task = std::make_shared<BluetoothTask>(logger, sender, loader, driver);
    BluetoothHandler handler(logger, task);

    EXPECT_CALL(*driver, scan()).Times(0);
    EXPECT_CALL(*loader, load_string_list(_)).Times(0);
    EXPECT_CALL(*loader, load_int(_)).Times(0);
    EXPECT_CALL(*loader, load_string(_)).Times(0);
    EXPECT_CALL(*sender, send()).Times(0);

    handler.handle(nullptr);

    EXPECT_EQ(task->state(), BluetoothTask::State::WaitRequest);
}

} // namespace device_reminder

