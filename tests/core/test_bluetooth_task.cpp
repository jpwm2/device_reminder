#include <gtest/gtest.h>
#include <vector>

#include "bluetooth_task/bluetooth_task.hpp"
#include "infra/bluetooth_driver/i_bluetooth_driver.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include "infra/file_loader/i_file_loader.hpp"

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

class StubSender : public IProcessSender {
public:
    int call_count = 0;
    void send() override { ++call_count; }
};

class StubLoader : public IFileLoader {
public:
    std::vector<std::string> list;
    int load_int() const override { return 0; }
    std::string load_string() const override { return {}; }
    std::vector<std::string> load_string_list() const override { return list; }
};

class DummyLogger : public ILogger {
public:
    void info(const std::string&) override {}
    void error(const std::string&) override {}
};

TEST(BluetoothTaskTest, SendsDetectedTrueWhenDeviceFound) {
    auto driver = std::make_shared<StubDriver>();
    auto sender = std::make_shared<StubSender>();
    auto loader = std::make_shared<StubLoader>();
    auto logger = std::make_shared<DummyLogger>();
    BluetoothTask task(logger, sender, loader, driver);

    driver->names = {"phone"};
    loader->list = {"phone"};

    task.on_waiting({});

    EXPECT_EQ(sender->call_count, 1);
    EXPECT_EQ(task.state(), BluetoothTask::State::WaitRequest);
}

TEST(BluetoothTaskTest, SendsDetectedFalseWhenNoDevice) {
    auto driver = std::make_shared<StubDriver>();
    auto sender = std::make_shared<StubSender>();
    auto loader = std::make_shared<StubLoader>();
    auto logger = std::make_shared<DummyLogger>();
    BluetoothTask task(logger, sender, loader, driver);

    driver->names = {};
    loader->list = {"phone"};
    task.on_waiting({});

    EXPECT_EQ(sender->call_count, 0);
    EXPECT_EQ(task.state(), BluetoothTask::State::WaitRequest);
}

TEST(BluetoothTaskTest, DriverErrorLogsAndSendsFalse) {
    auto driver = std::make_shared<StubDriver>();
    auto sender = std::make_shared<StubSender>();
    auto loader = std::make_shared<StubLoader>();
    auto logger = std::make_shared<DummyLogger>();
    BluetoothTask task(logger, sender, loader, driver);

    driver->fail = true;
    task.on_waiting({});

    EXPECT_EQ(sender->call_count, 0);
}

} // namespace device_reminder
