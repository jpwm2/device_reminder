#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/logger.h>
#include <thread>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <system_error>
#include <cerrno>

#include "infra/process_operation/message_codec/i_message_codec.hpp"
#include "infra/process_operation/process_queue/process_queue.hpp"
#include "infra/process_operation/process_receiver/process_receiver.hpp"
#include "infra/process_operation/process_dispatcher/process_dispatcher.hpp"
#include "infra/process_operation/process_sender/process_sender.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/process_operation/process_message/process_message_type.hpp"
#include "infra/file_loader/file_loader.hpp"
#include "infra/logger/logger.hpp"
#include "infra/watch_dog/watch_dog.hpp"
#include "infra/timer_service/i_timer_service.hpp"
#include "infra/bluetooth_driver/i_bluetooth_driver.hpp"
#include "core/bluetooth_task/bluetooth_task.hpp"
#include "core/bluetooth_task/bluetooth_handler.hpp"
#include "core/bluetooth_task/bluetooth_process.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

namespace device_reminder {

class MockMessageCodec : public IMessageCodec {
public:
    MOCK_METHOD(std::vector<uint8_t>, encode, (std::shared_ptr<IProcessMessage>), (override));
    MOCK_METHOD(std::shared_ptr<IProcessMessage>, decode, (const std::vector<uint8_t>&), (override));
};

class MockTimerService : public ITimerService {
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};

class MockBluetoothDriver : public IBluetoothDriver {
public:
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(std::vector<std::string>, scan, (), (override));
};

class ProcessBaseAccessor : public ProcessBase {
public:
    using ProcessBase::g_stop_flag;
};

class TestProcessQueue : public ProcessQueue {
public:
    using ProcessQueue::ProcessQueue;
    std::shared_ptr<IProcessMessage> pop() override {
        try {
            return ProcessQueue::pop();
        } catch (const std::system_error& e) {
            if (e.code().value() == EAGAIN) return nullptr;
            throw;
        }
    }
};
} // namespace device_reminder

using namespace device_reminder;

static std::string get_config_path() {
    namespace fs = std::filesystem;
    return (fs::path(__FILE__).parent_path().parent_path().parent_path().parent_path() / "data/bluetooth_process.conf").string();
}

TEST(BluetoothProcessIntegrationTest, 正常系_RequestBluetoothScan_デバイス検出) {
    ProcessBaseAccessor::g_stop_flag.store(false);

    auto oss = std::make_shared<std::ostringstream>();
    auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(*oss);
    auto spd = std::make_shared<spdlog::logger>("test", sink);
    auto logger = std::make_shared<Logger>(spd);

    auto codec = std::make_shared<StrictMock<MockMessageCodec>>();
    auto timer_service = std::make_shared<StrictMock<MockTimerService>>();
    auto driver = std::make_shared<StrictMock<MockBluetoothDriver>>();

    auto file_loader = std::make_shared<FileLoader>(logger, get_config_path());

    auto queue = std::make_shared<TestProcessQueue>(logger, codec, "/bp_queue");
    auto msg_for_send = std::make_shared<ProcessMessage>(ProcessMessageType::ResponseDevicePresence, std::vector<std::string>{});
    auto sender = std::make_shared<ProcessSender>(queue, msg_for_send);

    auto task = std::make_shared<BluetoothTask>(logger, sender, file_loader, driver);
    auto handler = std::make_shared<BluetoothHandler>(logger, task);

    ProcessDispatcher::HandlerMap handlers;
    handlers.emplace(ProcessMessageType::RequestBluetoothScan,
                     [handler](std::shared_ptr<IProcessMessage> m){ handler->handle(m); });
    auto dispatcher = std::make_shared<ProcessDispatcher>(logger, handlers);
    auto receiver = std::make_shared<ProcessReceiver>(logger, queue, dispatcher);
    auto watch_dog = std::make_shared<WatchDog>(timer_service);

    BluetoothProcess process(queue, receiver, dispatcher, sender, file_loader, logger, watch_dog, handler, task);

    auto req_msg = std::make_shared<ProcessMessage>(ProcessMessageType::RequestBluetoothScan, std::vector<std::string>{});

    EXPECT_CALL(*codec, encode(_)).Times(2).WillRepeatedly(Return(std::vector<uint8_t>{'a'}));
    EXPECT_CALL(*codec, decode(_))
        .WillOnce(Return(req_msg))
        .WillRepeatedly(Return(nullptr));
    EXPECT_CALL(*timer_service, start()).Times(1);
    EXPECT_CALL(*timer_service, stop()).Times(1);
    EXPECT_CALL(*driver, scan()).WillOnce(Return(std::vector<std::string>{"foo", "target"}));

    queue->push(req_msg);

    std::thread th([&]{ process.run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    process.stop();
    th.join();

    spd->flush();
}

TEST(BluetoothProcessIntegrationTest, 異常系_BluetoothDriver例外) {
    ProcessBaseAccessor::g_stop_flag.store(false);

    auto oss = std::make_shared<std::ostringstream>();
    auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(*oss);
    auto spd = std::make_shared<spdlog::logger>("test", sink);
    auto logger = std::make_shared<Logger>(spd);

    auto codec = std::make_shared<StrictMock<MockMessageCodec>>();
    auto timer_service = std::make_shared<StrictMock<MockTimerService>>();
    auto driver = std::make_shared<StrictMock<MockBluetoothDriver>>();

    auto file_loader = std::make_shared<FileLoader>(logger, get_config_path());

    auto queue = std::make_shared<TestProcessQueue>(logger, codec, "/bp_queue");
    auto msg_for_send = std::make_shared<ProcessMessage>(ProcessMessageType::ResponseDevicePresence, std::vector<std::string>{});
    auto sender = std::make_shared<ProcessSender>(queue, msg_for_send);

    auto task = std::make_shared<BluetoothTask>(logger, sender, file_loader, driver);
    auto handler = std::make_shared<BluetoothHandler>(logger, task);

    ProcessDispatcher::HandlerMap handlers;
    handlers.emplace(ProcessMessageType::RequestBluetoothScan,
                     [handler](std::shared_ptr<IProcessMessage> m){ handler->handle(m); });
    auto dispatcher = std::make_shared<ProcessDispatcher>(logger, handlers);
    auto receiver = std::make_shared<ProcessReceiver>(logger, queue, dispatcher);
    auto watch_dog = std::make_shared<WatchDog>(timer_service);

    BluetoothProcess process(queue, receiver, dispatcher, sender, file_loader, logger, watch_dog, handler, task);

    auto req_msg = std::make_shared<ProcessMessage>(ProcessMessageType::RequestBluetoothScan, std::vector<std::string>{});

    EXPECT_CALL(*codec, encode(_)).Times(1).WillRepeatedly(Return(std::vector<uint8_t>{'a'}));
    EXPECT_CALL(*codec, decode(_))
        .WillOnce(Return(req_msg))
        .WillRepeatedly(Return(nullptr));
    EXPECT_CALL(*timer_service, start()).Times(1);
    EXPECT_CALL(*timer_service, stop()).Times(1);
    EXPECT_CALL(*driver, scan()).WillOnce(::testing::Throw(BluetoothDriverError("scan failed")));

    queue->push(req_msg);

    std::thread th([&]{ process.run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    process.stop();
    th.join();

    spd->flush();
    EXPECT_NE(oss->str().find("scan failed"), std::string::npos);
}

