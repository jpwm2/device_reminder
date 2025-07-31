#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "bluetooth_task/bluetooth_handler.hpp"
#include "infra/process_operation/process_message/process_message.hpp"

using ::testing::StrictMock;

namespace device_reminder {

class MockBluetoothTask : public IBluetoothTask {
public:
    MOCK_METHOD(void, on_waiting, (const std::vector<std::string>&), (override));
};

class DummyLogger : public ILogger {
public:
    void info(const std::string&) override {}
    void error(const std::string&) override {}
    void warn(const std::string&) override {}
};

TEST(BluetoothHandlerTest, RequestScanCallsTask) {
    auto task = std::make_shared<StrictMock<MockBluetoothTask>>();
    auto logger = std::make_shared<DummyLogger>();
    BluetoothHandler handler(logger, task);

    EXPECT_CALL(*task, on_waiting(testing::_)).Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::RequestBluetoothScan, std::vector<std::string>{});
    handler.handle(msg);
}

TEST(BluetoothHandlerTest, OtherMessageIgnored) {
    auto task = std::make_shared<StrictMock<MockBluetoothTask>>();
    auto logger = std::make_shared<DummyLogger>();
    BluetoothHandler handler(logger, task);

    EXPECT_CALL(*task, on_waiting(testing::_)).Times(0);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartHumanDetection, std::vector<std::string>{});
    handler.handle(msg);
}

} // namespace device_reminder
