#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "core/bluetooth_task/bluetooth_handler.hpp"
#include "infra/message/message.hpp"

using ::testing::StrictMock;

namespace device_reminder {

class MockBluetoothTask : public IBluetoothTask {
public:
  MOCK_METHOD(void, on_waiting, (const std::vector<std::string> &), (override));
};

class DummyLogger : public ILogger {
public:
  void info(const std::string &) override {}
  void error(const std::string &) override {}
  void warn(const std::string &) override {}
};

class MockLogger : public ILogger {
public:
  MOCK_METHOD(void, info, (const std::string &), (override));
  MOCK_METHOD(void, error, (const std::string &), (override));
  MOCK_METHOD(void, warn, (const std::string &), (override));
};

TEST(BluetoothHandlerTest, RequestScanCallsTask) {
  auto task = std::make_shared<StrictMock<MockBluetoothTask>>();
  auto logger = std::make_shared<DummyLogger>();
  BluetoothHandler handler(logger, task);

  EXPECT_CALL(*task, on_waiting(testing::_)).Times(1);

  auto msg = std::make_shared<ProcessMessage>(
      ProcessMessageType::RequestBluetoothScan,
      std::vector<std::string>{},
      nullptr);
  handler.handle(msg);
}

TEST(BluetoothHandlerTest, OtherMessageIgnored) {
  auto task = std::make_shared<StrictMock<MockBluetoothTask>>();
  auto logger = std::make_shared<DummyLogger>();
  BluetoothHandler handler(logger, task);

  EXPECT_CALL(*task, on_waiting(testing::_)).Times(0);

  auto msg = std::make_shared<ProcessMessage>(
      ProcessMessageType::StartHumanDetection,
      std::vector<std::string>{},
      nullptr);
  handler.handle(msg);
}

TEST(BluetoothHandlerTest, ConstructorLogsWhenLoggerProvided) {
  auto task = std::make_shared<StrictMock<MockBluetoothTask>>();
  auto logger = std::make_shared<StrictMock<MockLogger>>();
  EXPECT_CALL(*logger, info("BluetoothHandler created")).Times(1);
  BluetoothHandler handler(logger, task);
}

TEST(BluetoothHandlerTest, ConstructorNoLogWhenLoggerNull) {
  auto task = std::make_shared<StrictMock<MockBluetoothTask>>();
  EXPECT_NO_THROW({ BluetoothHandler handler(nullptr, task); });
}

TEST(BluetoothHandlerTest, HandleNullMessageDoesNothing) {
  auto task = std::make_shared<StrictMock<MockBluetoothTask>>();
  auto logger = std::make_shared<DummyLogger>();
  BluetoothHandler handler(logger, task);
  EXPECT_CALL(*task, on_waiting(testing::_)).Times(0);
  handler.handle(nullptr);
}

TEST(BluetoothHandlerTest, HandleWithoutTaskDoesNothingAndNoLog) {
  auto logger = std::make_shared<StrictMock<MockLogger>>();
  EXPECT_CALL(*logger, info(testing::_)).Times(0);
  BluetoothHandler handler(logger, nullptr);
  auto msg = std::make_shared<ProcessMessage>(
      ProcessMessageType::RequestBluetoothScan,
      std::vector<std::string>{},
      nullptr);
  handler.handle(msg);
}

TEST(BluetoothHandlerTest, HandleLogsAndCallsTask) {
  auto task = std::make_shared<StrictMock<MockBluetoothTask>>();
  auto logger = std::make_shared<StrictMock<MockLogger>>();
  EXPECT_CALL(*logger, info("BluetoothHandler created")).Times(1);
  BluetoothHandler handler(logger, task);

  EXPECT_CALL(*logger, info("RequestBluetoothScan")).Times(1);
  EXPECT_CALL(*task, on_waiting(testing::_)).Times(1);

  auto msg = std::make_shared<ProcessMessage>(
      ProcessMessageType::RequestBluetoothScan,
      std::vector<std::string>{},
      nullptr);
  handler.handle(msg);
}

} // namespace device_reminder
