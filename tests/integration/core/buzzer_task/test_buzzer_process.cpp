#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <thread>
#include <future>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <ctime>

#include "buzzer_task/buzzer_process.hpp"
#include "infra/process_operation/process_receiver/process_receiver.hpp"
#include "infra/process_operation/process_sender/process_sender.hpp"
#include "infra/process_operation/process_queue/process_queue.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/process_operation/message_codec/i_message_codec.hpp"
#include "infra/process_operation/process_dispatcher/i_process_dispatcher.hpp"
#include "infra/watch_dog/watch_dog.hpp"
#include "infra/timer_service/i_timer_service.hpp"
#include "infra/file_loader/file_loader.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/process_operation/process_base/process_base.hpp"
#include "posix_mq_stub.h"

using namespace device_reminder;
using ::testing::StrictMock;
using ::testing::Return;
using ::testing::_;

namespace {

class DummyLogger : public ILogger {
public:
    void info(const std::string&) override {}
    void error(const std::string&) override {}
    void warn(const std::string&) override {}
};

class MockMessageCodec : public IMessageCodec {
public:
    MOCK_METHOD(std::vector<uint8_t>, encode,
                (std::shared_ptr<IProcessMessage> msg), (override));
    MOCK_METHOD(std::shared_ptr<IProcessMessage>, decode,
                (const std::vector<uint8_t>& data), (override));
};

class MockDispatcher : public IProcessDispatcher {
public:
    MOCK_METHOD(void, dispatch, (std::shared_ptr<IProcessMessage> msg), (override));
};

class MockTimerService : public ITimerService {
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};

class ProcessBaseAccessor : public ProcessBase {
public:
    ProcessBaseAccessor()
        : ProcessBase(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "test") {}
    static void Reset() { g_stop_flag.store(false); }
};

static std::string unique_queue_name() {
    return "/buzzer_" + std::to_string(::getpid()) + std::to_string(::time(nullptr));
}

} // namespace

TEST(BuzzerProcessIntegrationTest, 正常系メッセージをディスパッチする) {
    ProcessBaseAccessor::Reset();
    mq_stub_reset();

    auto logger = std::make_shared<DummyLogger>();
    auto codec = std::make_shared<StrictMock<MockMessageCodec>>();
    auto dispatcher = std::make_shared<StrictMock<MockDispatcher>>();
    auto timer = std::make_shared<StrictMock<MockTimerService>>();

    auto queue_name = unique_queue_name();
    auto queue = std::make_shared<ProcessQueue>(logger, codec, queue_name);
    auto receiver = std::make_shared<ProcessReceiver>(logger, queue, dispatcher);
    auto message = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{});
    std::shared_ptr<IProcessMessage> msg = message;
    auto sender = std::make_shared<ProcessSender>(queue, msg);

    std::string config = "/tmp/buzzer_test.cfg";
    {
        std::ofstream ofs(config);
        ofs << "priority=0\n";
    }
    auto loader = std::make_shared<FileLoader>(logger, config);
    auto watch_dog = std::make_shared<WatchDog>(timer);

    BuzzerProcess proc(receiver, sender, queue, loader, logger, watch_dog);

    std::vector<uint8_t> encoded{1, 2, 3};
    EXPECT_CALL(*codec, encode(_)).WillOnce(Return(encoded));
    EXPECT_CALL(*codec, decode(encoded)).WillOnce(Return(msg));

    std::promise<void> dispatched;
    EXPECT_CALL(*dispatcher, dispatch(_))
        .WillOnce([&](std::shared_ptr<IProcessMessage> m){
            EXPECT_EQ(m, msg);
            std::thread([receiver]{ receiver->stop(); }).detach();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            dispatched.set_value();
        });

    EXPECT_CALL(*timer, start()).Times(1);
    EXPECT_CALL(*timer, stop()).Times(1);

    int run_result = -1;
    std::thread th([&]{ run_result = proc.run(); });

    sender->send();
    dispatched.get_future().wait();

    proc.stop();
    th.join();

    EXPECT_EQ(run_result, 0);
    ProcessBaseAccessor::Reset();
}

TEST(BuzzerProcessIntegrationTest, 異常系デコード失敗ではディスパッチしない) {
    ProcessBaseAccessor::Reset();
    mq_stub_reset();

    auto logger = std::make_shared<DummyLogger>();
    auto codec = std::make_shared<StrictMock<MockMessageCodec>>();
    auto dispatcher = std::make_shared<StrictMock<MockDispatcher>>();
    auto timer = std::make_shared<StrictMock<MockTimerService>>();

    auto queue_name = unique_queue_name();
    auto queue = std::make_shared<ProcessQueue>(logger, codec, queue_name);
    auto receiver = std::make_shared<ProcessReceiver>(logger, queue, dispatcher);
    auto message = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{});
    std::shared_ptr<IProcessMessage> msg = message;
    auto sender = std::make_shared<ProcessSender>(queue, msg);

    std::string config = "/tmp/buzzer_test.cfg";
    {
        std::ofstream ofs(config);
        ofs << "priority=0\n";
    }
    auto loader = std::make_shared<FileLoader>(logger, config);
    auto watch_dog = std::make_shared<WatchDog>(timer);

    BuzzerProcess proc(receiver, sender, queue, loader, logger, watch_dog);

    std::vector<uint8_t> encoded{1, 2, 3};
    EXPECT_CALL(*codec, encode(_)).WillOnce(Return(encoded));

    std::promise<void> decoded;
    EXPECT_CALL(*codec, decode(encoded))
        .WillOnce([&](const std::vector<uint8_t>&){
            std::thread([receiver]{ receiver->stop(); }).detach();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            decoded.set_value();
            return std::shared_ptr<IProcessMessage>(nullptr);
        });

    EXPECT_CALL(*dispatcher, dispatch(_)).Times(0);
    EXPECT_CALL(*timer, start()).Times(1);
    EXPECT_CALL(*timer, stop()).Times(1);

    int run_result = -1;
    std::thread th([&]{ run_result = proc.run(); });

    sender->send();
    decoded.get_future().wait();

    proc.stop();
    th.join();

    EXPECT_EQ(run_result, 0);
    ProcessBaseAccessor::Reset();
}

