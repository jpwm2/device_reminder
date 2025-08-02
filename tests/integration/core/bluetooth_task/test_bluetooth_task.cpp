#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <memory>

#include "core/bluetooth_task/bluetooth_task.hpp"
#include "infra/logger/logger.hpp"
#include "infra/process_operation/process_sender/process_sender.hpp"
#include "infra/file_loader/file_loader.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/process_operation/process_message/i_process_message.hpp"
#include "infra/bluetooth_driver/bluetooth_driver.hpp"
#include <spdlog/sinks/null_sink.h>

extern "C" {
#include "popen_stub.h"
}

using namespace testing;
using namespace device_reminder;

// 1段下モジュールのモック
class MockQueue : public IProcessQueue {
public:
    MOCK_METHOD(void, push, (std::shared_ptr<IProcessMessage>), (override));
    MOCK_METHOD(std::shared_ptr<IProcessMessage>, pop, (), (override));
    MOCK_METHOD(std::size_t, size, (), (const, override));
};

// 送信メッセージのダミー
class DummyMessage : public IProcessMessage {
public:
    ProcessMessageType type() const noexcept override { return ProcessMessageType::RequestBluetoothScan; }
    std::vector<std::string> payload() const noexcept override { return {}; }
    std::shared_ptr<IProcessMessage> clone() const override { return std::make_shared<DummyMessage>(*this); }
    std::string to_string() const override { return {}; }
};

// 正常系: 登録済みデバイス検出時にメッセージ送信
TEST(BluetoothTaskIntegration, SendWhenRegisteredDeviceFound) {
    popen_stub_reset();
    popen_stub_set_output("Scanning ...\n00:00:00:00:00\tphone\n");

    auto null_sink = std::make_shared<spdlog::sinks::null_sink_st>();
    auto spd = std::make_shared<spdlog::logger>("test", null_sink);
    auto logger = std::make_shared<Logger>(spd);

    std::ofstream ofs("devices.conf");
    ofs << "device_list=phone\n";
    ofs.close();
    auto loader = std::make_shared<FileLoader>(logger, "devices.conf");

    auto queue = std::make_shared<StrictMock<MockQueue>>();
    auto msg = std::make_shared<DummyMessage>();
    auto sender = std::make_shared<ProcessSender>(queue, msg);

    auto driver = std::make_shared<BluetoothDriver>(*logger);
    BluetoothTask task(logger, sender, loader, driver);

    EXPECT_CALL(*queue, push(_)).Times(1);
    task.on_waiting({});
    EXPECT_EQ(task.state(), BluetoothTask::State::WaitRequest);
}

// 異常系: スキャン失敗時はメッセージ送信なし
TEST(BluetoothTaskIntegration, NoSendWhenDriverFails) {
    popen_stub_reset();
    popen_stub_set_fail(1);

    auto null_sink = std::make_shared<spdlog::sinks::null_sink_st>();
    auto spd = std::make_shared<spdlog::logger>("test", null_sink);
    auto logger = std::make_shared<Logger>(spd);

    std::ofstream ofs("devices.conf");
    ofs << "device_list=phone\n";
    ofs.close();
    auto loader = std::make_shared<FileLoader>(logger, "devices.conf");

    auto queue = std::make_shared<StrictMock<MockQueue>>();
    auto msg = std::make_shared<DummyMessage>();
    auto sender = std::make_shared<ProcessSender>(queue, msg);

    auto driver = std::make_shared<BluetoothDriver>(*logger);
    BluetoothTask task(logger, sender, loader, driver);

    EXPECT_CALL(*queue, push(_)).Times(0);
    task.on_waiting({"dummy"});
    EXPECT_EQ(task.state(), BluetoothTask::State::WaitRequest);
}

