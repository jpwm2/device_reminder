#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <fstream>
#include <thread>
#include <chrono>
#include <memory>
#include <vector>

#include "core/main_task/main_process.hpp"
#include "infra/process_operation/process_sender/process_sender.hpp"
#include "infra/process_operation/process_receiver/process_receiver.hpp"
#include "infra/process_operation/process_queue/process_queue.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/process_operation/process_dispatcher/i_process_dispatcher.hpp"
#include "infra/process_operation/message_codec/i_message_codec.hpp"
#include "infra/watch_dog/watch_dog.hpp"
#include "infra/timer_service/i_timer_service.hpp"
#include "infra/file_loader/file_loader.hpp"
#include "infra/logger/logger.hpp"

extern "C" {
#include "posix_mq_stub.h"
}

#include <spdlog/sinks/null_sink.h>

using ::testing::_;
using ::testing::StrictMock;
using ::testing::Return;
using ::testing::Property;

using namespace device_reminder;

class MockTimerService : public ITimerService {
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};

class MockProcessDispatcher : public IProcessDispatcher {
public:
    MOCK_METHOD(void, dispatch, (std::shared_ptr<IProcessMessage> msg), (override));
};

class MockMessageCodec : public IMessageCodec {
public:
    MOCK_METHOD(std::vector<uint8_t>, encode, (std::shared_ptr<IProcessMessage> msg), (override));
    MOCK_METHOD(std::shared_ptr<IProcessMessage>, decode, (const std::vector<uint8_t>& data), (override));
};

class TestMainProcess : public MainProcess {
public:
    using ProcessBase::g_stop_flag;
    using MainProcess::MainProcess;
};

static std::shared_ptr<Logger> make_logger() {
    auto sink = std::make_shared<spdlog::sinks::null_sink_mt>();
    auto spd = std::make_shared<spdlog::logger>("null", sink);
    return std::make_shared<Logger>(spd);
}

TEST(統合テスト_main_process, 正常系メッセージを処理できること) {
    mq_stub_reset();
    TestMainProcess::g_stop_flag.store(false);

    auto logger = make_logger();

    auto timer_service = std::make_shared<StrictMock<MockTimerService>>();
    auto watchdog = std::make_shared<WatchDog>(timer_service);

    auto dispatcher = std::make_shared<StrictMock<MockProcessDispatcher>>();
    auto codec = std::make_shared<StrictMock<MockMessageCodec>>();
    auto queue = std::make_shared<ProcessQueue>(logger, codec, "/main_proc_queue1");

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{});
    auto sender = std::make_shared<ProcessSender>(queue, msg);
    auto receiver = std::make_shared<ProcessReceiver>(logger, queue, dispatcher);

    std::string cfg = "/tmp/main_process.cfg";
    { std::ofstream ofs(cfg); ofs << "priority=1\n"; }
    auto loader = std::make_shared<FileLoader>(logger, cfg);

    TestMainProcess proc(receiver, sender, queue, loader, logger, watchdog);

    EXPECT_CALL(*codec, encode(_)).WillOnce(Return(std::vector<uint8_t>{1}));
    EXPECT_CALL(*codec, decode(_))
        .WillOnce(Return(msg))
        .WillRepeatedly(Return(nullptr));
    EXPECT_CALL(*dispatcher,
                dispatch(testing::Truly([](const std::shared_ptr<IProcessMessage>& m) {
                    return m && m->type() == ProcessMessageType::StartBuzzing;
                })))
        .Times(1);
    EXPECT_CALL(*timer_service, start()).Times(1);
    EXPECT_CALL(*timer_service, stop()).Times(1);

    sender->send();

    std::thread th([&]{ proc.run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    proc.stop();
    th.join();
}

TEST(統合テスト_main_process, 未定義メッセージを投入した場合にディスパッチされないこと) {
    mq_stub_reset();
    TestMainProcess::g_stop_flag.store(false);

    auto logger = make_logger();

    auto timer_service = std::make_shared<StrictMock<MockTimerService>>();
    auto watchdog = std::make_shared<WatchDog>(timer_service);

    auto dispatcher = std::make_shared<StrictMock<MockProcessDispatcher>>();
    auto codec = std::make_shared<StrictMock<MockMessageCodec>>();
    auto queue = std::make_shared<ProcessQueue>(logger, codec, "/main_proc_queue2");

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::BuzzTimeout, std::vector<std::string>{});
    auto sender = std::make_shared<ProcessSender>(queue, msg);
    auto receiver = std::make_shared<ProcessReceiver>(logger, queue, dispatcher);

    std::string cfg = "/tmp/main_process.cfg";
    { std::ofstream ofs(cfg); ofs << "priority=1\n"; }
    auto loader = std::make_shared<FileLoader>(logger, cfg);

    TestMainProcess proc(receiver, sender, queue, loader, logger, watchdog);

    EXPECT_CALL(*codec, encode(_)).WillOnce(Return(std::vector<uint8_t>{2}));
    EXPECT_CALL(*codec, decode(_)).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(*dispatcher, dispatch(_)).Times(0);
    EXPECT_CALL(*timer_service, start()).Times(1);
    EXPECT_CALL(*timer_service, stop()).Times(1);

    sender->send();

    std::thread th([&]{ proc.run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    proc.stop();
    th.join();
}

