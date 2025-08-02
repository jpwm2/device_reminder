#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <fstream>
#include <thread>
#include <chrono>

#include "core/human_task/human_process.hpp"
#include "core/human_task/human_handler.hpp"
#include "core/human_task/human_task.hpp"

#include "infra/process_operation/process_queue/process_queue.hpp"
#include "infra/process_operation/process_dispatcher/process_dispatcher.hpp"
#include "infra/process_operation/process_sender/process_sender.hpp"
#include "infra/process_operation/process_receiver/process_receiver.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/process_operation/message_codec/i_message_codec.hpp"
#include "infra/process_operation/process_base/process_base.hpp"
#include "infra/file_loader/file_loader.hpp"
#include "infra/logger/logger.hpp"
#include "infra/watch_dog/watch_dog.hpp"
#include "infra/timer_service/i_timer_service.hpp"
#include "infra/pir_driver/i_pir_driver.hpp"

using ::testing::Return;
using ::testing::NiceMock;
using ::testing::StrictMock;
using ::testing::HasSubstr;

namespace device_reminder {

class MockMessageCodec : public IMessageCodec {
public:
    MOCK_METHOD(std::vector<uint8_t>, encode, (std::shared_ptr<IProcessMessage> msg), (override));
    MOCK_METHOD(std::shared_ptr<IProcessMessage>, decode, (const std::vector<uint8_t>& data), (override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

class MockTimerService : public ITimerService {
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};

class MockPIRDriver : public IPIRDriver {
public:
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};

class MockProcessSender : public IProcessSender {
public:
    MOCK_METHOD(void, send, (), (override));
};

class ProcessBaseAccessor : public ProcessBase {
public:
    using ProcessBase::g_stop_flag;
    ProcessBaseAccessor()
        : ProcessBase(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "Accessor") {}
};

// 正常系テスト
TEST(HumanProcessIntegrationTest, RunWithStartHumanDetection) {
    ProcessBaseAccessor::g_stop_flag.store(false);

    auto codec = std::make_shared<StrictMock<MockMessageCodec>>();
    auto queue_logger = std::make_shared<NiceMock<MockLogger>>();
    auto receiver_logger = std::make_shared<NiceMock<MockLogger>>();
    auto dispatcher_logger = std::make_shared<NiceMock<MockLogger>>();
    auto file_logger = std::make_shared<NiceMock<MockLogger>>();
    auto handler_logger = std::make_shared<NiceMock<MockLogger>>();
    auto task_logger = std::make_shared<NiceMock<MockLogger>>();
    auto timer = std::make_shared<StrictMock<MockTimerService>>();
    auto pir = std::make_shared<StrictMock<MockPIRDriver>>();
    auto task_sender = std::make_shared<NiceMock<MockProcessSender>>();

    // ファイル準備
    std::string file_path = "/tmp/human_process_integration1.conf";
    {
        std::ofstream ofs(file_path);
        ofs << "priority=0\n";
    }
    auto file_loader = std::make_shared<FileLoader>(file_logger, file_path);

    auto queue = std::make_shared<ProcessQueue>(queue_logger, codec, "/hp_queue1");

    auto task = std::make_shared<HumanTask>(task_logger, pir, task_sender);
    auto handler = std::make_shared<HumanHandler>(handler_logger, task, timer);

    ProcessDispatcher::HandlerMap map = {
        {ProcessMessageType::StartHumanDetection,
         [handler](std::shared_ptr<IProcessMessage> msg) { handler->handle(std::move(msg)); }}
    };
    auto dispatcher = std::make_shared<ProcessDispatcher>(dispatcher_logger, map);
    auto receiver = std::make_shared<ProcessReceiver>(receiver_logger, queue, dispatcher);

    auto dummy_msg = std::make_shared<ProcessMessage>(ProcessMessageType::CooldownTimeout, std::vector<std::string>{});
    auto proc_sender = std::make_shared<ProcessSender>(queue, dummy_msg);

    auto spd = spdlog::stdout_color_mt("human_process_int1");
    auto main_logger = std::make_shared<Logger>(spd);
    auto watch_dog = std::make_shared<WatchDog>(timer);

    HumanProcess proc(queue, receiver, dispatcher, proc_sender, file_loader, main_logger, watch_dog, handler, task);

    std::shared_ptr<IProcessMessage> start_msg =
        std::make_shared<ProcessMessage>(ProcessMessageType::StartHumanDetection, std::vector<std::string>{});

    EXPECT_CALL(*codec, encode(start_msg)).WillOnce(Return(std::vector<uint8_t>{1}));
    EXPECT_CALL(*codec, decode(testing::_))
        .WillOnce(Return(start_msg))
        .WillRepeatedly(Return(nullptr));

    EXPECT_CALL(*timer, start()).Times(1);
    EXPECT_CALL(*timer, stop()).Times(2);
    EXPECT_CALL(*pir, run()).Times(1);
    EXPECT_CALL(*receiver_logger, info(HasSubstr("ProcessReceiver loop end"))).Times(1);
    EXPECT_CALL(*receiver_logger, info(HasSubstr("ProcessReceiver stopped"))).Times(1);

    auto sender = std::make_shared<ProcessSender>(queue, start_msg);
    sender->send();

    std::thread th([&] { proc.run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    proc.stop();
    th.join();
}

// 異常系テスト
TEST(HumanProcessIntegrationTest, RunWithDecodeFailure) {
    ProcessBaseAccessor::g_stop_flag.store(false);

    auto codec = std::make_shared<StrictMock<MockMessageCodec>>();
    auto queue_logger = std::make_shared<NiceMock<MockLogger>>();
    auto receiver_logger = std::make_shared<NiceMock<MockLogger>>();
    auto dispatcher_logger = std::make_shared<NiceMock<MockLogger>>();
    auto file_logger = std::make_shared<NiceMock<MockLogger>>();
    auto handler_logger = std::make_shared<NiceMock<MockLogger>>();
    auto task_logger = std::make_shared<NiceMock<MockLogger>>();
    auto timer = std::make_shared<StrictMock<MockTimerService>>();
    auto pir = std::make_shared<StrictMock<MockPIRDriver>>();
    auto task_sender = std::make_shared<NiceMock<MockProcessSender>>();

    std::string file_path = "/tmp/human_process_integration2.conf";
    {
        std::ofstream ofs(file_path);
        ofs << "priority=0\n";
    }
    auto file_loader = std::make_shared<FileLoader>(file_logger, file_path);

    auto queue = std::make_shared<ProcessQueue>(queue_logger, codec, "/hp_queue2");

    auto task = std::make_shared<HumanTask>(task_logger, pir, task_sender);
    auto handler = std::make_shared<HumanHandler>(handler_logger, task, timer);

    ProcessDispatcher::HandlerMap map = {
        {ProcessMessageType::StartHumanDetection,
         [handler](std::shared_ptr<IProcessMessage> msg) { handler->handle(std::move(msg)); }}
    };
    auto dispatcher = std::make_shared<ProcessDispatcher>(dispatcher_logger, map);
    auto receiver = std::make_shared<ProcessReceiver>(receiver_logger, queue, dispatcher);

    auto dummy_msg = std::make_shared<ProcessMessage>(ProcessMessageType::CooldownTimeout, std::vector<std::string>{});
    auto proc_sender = std::make_shared<ProcessSender>(queue, dummy_msg);

    auto spd = spdlog::stdout_color_mt("human_process_int2");
    auto main_logger = std::make_shared<Logger>(spd);
    auto watch_dog = std::make_shared<WatchDog>(timer);

    HumanProcess proc(queue, receiver, dispatcher, proc_sender, file_loader, main_logger, watch_dog, handler, task);

    std::shared_ptr<IProcessMessage> start_msg =
        std::make_shared<ProcessMessage>(ProcessMessageType::StartHumanDetection, std::vector<std::string>{});

    EXPECT_CALL(*codec, encode(start_msg)).WillOnce(Return(std::vector<uint8_t>{1}));
    EXPECT_CALL(*codec, decode(testing::_)).WillRepeatedly(Return(nullptr));

    EXPECT_CALL(*timer, start()).Times(1);
    EXPECT_CALL(*timer, stop()).Times(1);
    EXPECT_CALL(*pir, run()).Times(0);
    EXPECT_CALL(*receiver_logger, info(HasSubstr("ProcessReceiver loop end"))).Times(1);
    EXPECT_CALL(*receiver_logger, info(HasSubstr("ProcessReceiver stopped"))).Times(1);

    auto sender = std::make_shared<ProcessSender>(queue, start_msg);
    sender->send();

    std::thread th([&] { proc.run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    proc.stop();
    th.join();
}

} // namespace device_reminder
