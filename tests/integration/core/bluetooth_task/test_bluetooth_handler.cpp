#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "core/bluetooth_task/bluetooth_handler.hpp"
#include "core/bluetooth_task/bluetooth_task.hpp"
#include "infra/bluetooth_driver/i_bluetooth_driver.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::StrictMock;

namespace device_reminder {

class MockProcessSender : public IProcessSender {
public:
  MOCK_METHOD(void, send, (), (override));
};

class MockBluetoothDriver : public IBluetoothDriver {
public:
  MOCK_METHOD(void, run, (), (override));
  MOCK_METHOD(void, stop, (), (override));
  MOCK_METHOD(const std::vector<std::string> &, paired_devices, (),
              (const, override));
};

class DummyLogger : public ILogger {
public:
  void info(const std::string &) override {}
  void error(const std::string &) override {}
  void warn(const std::string &) override {}
};

TEST(BluetoothHandlerIntegrationTest,
     RequestBluetoothScanTriggersSendWhenDeviceMatches) {
  auto sender = std::make_shared<StrictMock<MockProcessSender>>();
  auto driver = std::make_shared<StrictMock<MockBluetoothDriver>>();
  auto logger = std::make_shared<DummyLogger>();
  auto task = std::make_shared<BluetoothTask>(logger, sender, driver);
  BluetoothHandler handler(logger, task);

  std::vector<std::string> paired{"RegisteredDevice"};
  {
    InSequence seq;
    EXPECT_CALL(*driver, run()).WillOnce([&]() {});
    EXPECT_CALL(*driver, paired_devices()).WillRepeatedly(ReturnRef(paired));
    EXPECT_CALL(*sender, send()).Times(1);
  }

  auto msg = std::make_shared<ProcessMessage>(
      ProcessMessageType::RequestBluetoothScan, std::vector<std::string>{});
  handler.handle(msg);

  EXPECT_EQ(task->state(), BluetoothTask::State::WaitRequest);
}

TEST(BluetoothHandlerIntegrationTest, NullMessageDoesNothing) {
  auto sender = std::make_shared<StrictMock<MockProcessSender>>();
  auto driver = std::make_shared<StrictMock<MockBluetoothDriver>>();
  auto logger = std::make_shared<DummyLogger>();
  auto task = std::make_shared<BluetoothTask>(logger, sender, driver);
  BluetoothHandler handler(logger, task);

  EXPECT_CALL(*driver, run()).Times(0);
  EXPECT_CALL(*driver, paired_devices()).Times(0);
  EXPECT_CALL(*sender, send()).Times(0);

  handler.handle(nullptr);

  EXPECT_EQ(task->state(), BluetoothTask::State::WaitRequest);
}

} // namespace device_reminder
