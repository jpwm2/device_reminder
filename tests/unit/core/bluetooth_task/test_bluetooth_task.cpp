#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

#include "bluetooth_task/bluetooth_task.hpp"
#include "infra/bluetooth_driver/i_bluetooth_driver.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"

using namespace std;

namespace device_reminder {

class StubDriver : public IBluetoothDriver {
public:
  vector<string> paired{};
  bool fail = false;
  int call_count = 0;
  void run() override {
    ++call_count;
    if (fail)
      throw BluetoothDriverError("fail");
  }
  void stop() override {}
  const vector<string> &paired_devices() const override { return paired; }
};

class StubSender : public IProcessSender {
public:
  int call_count = 0;
  void send() override { ++call_count; }
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

TEST(BluetoothTaskTest, SendsDetectedTrueWhenDeviceFound) {
  auto driver = std::make_shared<StubDriver>();
  auto sender = std::make_shared<StubSender>();
  auto logger = std::make_shared<DummyLogger>();
  BluetoothTask task(logger, sender, driver);

  driver->paired = {"phone"};

  task.on_waiting({});

  EXPECT_EQ(sender->call_count, 1);
  EXPECT_EQ(task.state(), BluetoothTask::State::WaitRequest);
}

TEST(BluetoothTaskTest, SendsDetectedFalseWhenNoDevice) {
  auto driver = std::make_shared<StubDriver>();
  auto sender = std::make_shared<StubSender>();
  auto logger = std::make_shared<DummyLogger>();
  BluetoothTask task(logger, sender, driver);

  driver->paired = {};
  task.on_waiting({});

  EXPECT_EQ(sender->call_count, 0);
  EXPECT_EQ(task.state(), BluetoothTask::State::WaitRequest);
}

TEST(BluetoothTaskTest, DriverErrorLogsAndSendsFalse) {
  auto driver = std::make_shared<StubDriver>();
  auto sender = std::make_shared<StubSender>();
  auto logger = std::make_shared<DummyLogger>();
  BluetoothTask task(logger, sender, driver);

  driver->fail = true;
  task.on_waiting({});

  EXPECT_EQ(sender->call_count, 0);
}

TEST(BluetoothTaskTest, ConstructorLogsWhenLoggerGiven) {
  auto driver = std::make_shared<StubDriver>();
  auto sender = std::make_shared<StubSender>();
  auto logger = std::make_shared<testing::StrictMock<MockLogger>>();

  EXPECT_CALL(*logger, info("BluetoothTask created")).Times(1);
  BluetoothTask task(logger, sender, driver);
}

TEST(BluetoothTaskTest, DestructorLogsWhenLoggerGiven) {
  auto driver = std::make_shared<StubDriver>();
  auto sender = std::make_shared<StubSender>();
  auto logger = std::make_shared<testing::StrictMock<MockLogger>>();

  {
    EXPECT_CALL(*logger, info("BluetoothTask created")).Times(1);
    EXPECT_CALL(*logger, info("BluetoothTask destroyed")).Times(1);
    BluetoothTask task(logger, sender, driver);
  }
}

TEST(BluetoothTaskTest, OnWaitingNoSendWhenDriverNull) {
  std::shared_ptr<StubDriver> driver = nullptr;
  auto sender = std::make_shared<StubSender>();
  auto logger = std::make_shared<DummyLogger>();

  BluetoothTask task(logger, sender, driver);
  task.on_waiting({});

  EXPECT_EQ(sender->call_count, 0);
}

TEST(BluetoothTaskTest, OnWaitingNoSendWhenSenderNull) {
  auto driver = std::make_shared<StubDriver>();
  std::shared_ptr<StubSender> sender = nullptr;
  auto logger = std::make_shared<DummyLogger>();

  BluetoothTask task(logger, sender, driver);
  driver->paired = {"phone"};

  EXPECT_NO_THROW(task.on_waiting({}));
}

TEST(BluetoothTaskTest, DriverExceptionLogsError) {
  auto driver = std::make_shared<StubDriver>();
  auto sender = std::make_shared<StubSender>();
  auto logger = std::make_shared<testing::StrictMock<MockLogger>>();
  BluetoothTask task(logger, sender, driver);

  driver->fail = true;
  EXPECT_CALL(*logger, error("fail"));
  task.on_waiting({});

  EXPECT_EQ(sender->call_count, 0);
}

TEST(BluetoothTaskTest, DriverExceptionWithoutLogger) {
  auto driver = std::make_shared<StubDriver>();
  auto sender = std::make_shared<StubSender>();
  std::shared_ptr<ILogger> logger = nullptr;
  BluetoothTask task(logger, sender, driver);

  driver->fail = true;
  EXPECT_NO_THROW(task.on_waiting({}));
}

} // namespace device_reminder
