#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "core/bluetooth_task/bluetooth_dispatcher.hpp"
#include "core/bluetooth_task/bluetooth_handler.hpp"
#include "infra/message/message.hpp"

namespace device_reminder {

class MockBluetoothHandler : public IBluetoothHandler {
public:
    MOCK_METHOD(std::vector<std::string>, scan, (), (override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

} // namespace device_reminder

using namespace device_reminder;
using ::testing::StrictMock;
using ::testing::Return;

TEST(BluetoothDispatcherTest, ExecutesScanWhenReady) {
    auto handler = std::make_shared<StrictMock<MockBluetoothHandler>>();
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    BluetoothDispatcher dispatcher(logger, handler, MessageType::RequestBluetoothScan);

    EXPECT_CALL(*handler, scan()).WillOnce(Return(std::vector<std::string>{}));
    EXPECT_CALL(*logger, info(testing::_)).Times(testing::AtLeast(1));

    auto msg = std::make_shared<Message>(MessageType::RequestBluetoothScan, std::vector<std::string>{}, nullptr);
    dispatcher.dispatch(msg);
}

TEST(BluetoothDispatcherTest, IgnoreWhenStateNotReady) {
    auto handler = std::make_shared<StrictMock<MockBluetoothHandler>>();
    BluetoothDispatcher dispatcher(nullptr, handler, MessageType::RequestBluetoothScan);

    auto msg = std::make_shared<Message>(MessageType::RequestBluetoothScan, std::vector<std::string>{}, nullptr);
    EXPECT_CALL(*handler, scan()).WillOnce(Return(std::vector<std::string>{}));
    dispatcher.dispatch(msg); // first call changes state to Cooldown
    EXPECT_CALL(*handler, scan()).Times(0);
    dispatcher.dispatch(msg); // ignored due to Cooldown
}
