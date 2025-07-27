#include <gtest/gtest.h>
#include <vector>

#include "bluetooth_task/bluetooth_task.hpp"
#include "infra/bluetooth_driver/i_bluetooth_driver.hpp"
#include "process_message_operation/i_process_message_sender.hpp"
#include "thread_message_operation/thread_message.hpp"

using namespace std;

namespace device_reminder {

class StubDriver : public IBluetoothDriver {
public:
    vector<string> names{};
    bool fail = false;
    int call_count = 0;
    std::vector<std::string> scan() override {
        ++call_count;
        if (fail) throw BluetoothDriverError("fail");
        return names;
    }
};

class StubSender : public IProcessMessageSender {
public:
    std::vector<ProcessMessage> sent;
    bool enqueue(const ProcessMessage& m) override {
        sent.push_back(m);
        return true;
    }
    void stop() override {}
};

class DummyLogger : public ILogger {
public:
    void info(const std::string&) override {}
    void error(const std::string&) override {}
};

TEST(BluetoothTaskTest, SendsDetectedTrueWhenDeviceFound) {
    auto driver = std::make_shared<StubDriver>();
    auto sender = std::make_shared<StubSender>();
    auto logger = std::make_shared<DummyLogger>();
    BluetoothTask task(driver, sender, logger);

    driver->names = {"phone"};

    task.run(ThreadMessage{ThreadMessageType::BluetoothScanRequested});

    ASSERT_EQ(sender->sent.size(), 1u);
    EXPECT_EQ(sender->sent[0].payload_, true);
    EXPECT_EQ(task.state(), BluetoothTask::State::WaitRequest);
}

TEST(BluetoothTaskTest, SendsDetectedFalseWhenNoDevice) {
    auto driver = std::make_shared<StubDriver>();
    auto sender = std::make_shared<StubSender>();
    auto logger = std::make_shared<DummyLogger>();
    BluetoothTask task(driver, sender, logger);

    driver->names = {};
    task.run(ThreadMessage{ThreadMessageType::BluetoothScanRequested});

    ASSERT_EQ(sender->sent.size(), 1u);
    EXPECT_EQ(sender->sent[0].payload_, false);
    EXPECT_EQ(task.state(), BluetoothTask::State::WaitRequest);
}

TEST(BluetoothTaskTest, DriverErrorLogsAndSendsFalse) {
    auto driver = std::make_shared<StubDriver>();
    auto sender = std::make_shared<StubSender>();
    auto logger = std::make_shared<DummyLogger>();
    BluetoothTask task(driver, sender, logger);

    driver->fail = true;
    task.run(ThreadMessage{ThreadMessageType::BluetoothScanRequested});

    ASSERT_EQ(sender->sent.size(), 1u);
    EXPECT_EQ(sender->sent[0].payload_, false);
}

} // namespace device_reminder
