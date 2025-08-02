// Copyright 2024

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <fstream>
#include <future>
#include <thread>
#include <unistd.h>
#include <cstdio>

#include "tests/stubs/posix_mq_stub.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "infra/logger/logger.hpp"
#include "infra/process_operation/message_codec/i_message_codec.hpp"
#include "infra/process_operation/process_base/process_base.hpp"
#include "infra/process_operation/process_dispatcher/process_dispatcher.hpp"
#include "infra/process_operation/process_queue/process_queue.hpp"
#include "infra/process_operation/process_receiver/process_receiver.hpp"
#include "infra/process_operation/process_sender/process_sender.hpp"
#include "infra/file_loader/file_loader.hpp"

using namespace device_reminder;
using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

namespace {

class MockMessageCodec : public IMessageCodec {
public:
    MOCK_METHOD(std::vector<uint8_t>, encode,
                (std::shared_ptr<IProcessMessage>), (override));
    MOCK_METHOD(std::shared_ptr<IProcessMessage>, decode,
                (const std::vector<uint8_t>&), (override));
};

class MockProcessMessage : public IProcessMessage {
public:
    MOCK_METHOD(ProcessMessageType, type, (), (const, noexcept, override));
    MOCK_METHOD(std::vector<std::string>, payload, (), (const, noexcept, override));
    MOCK_METHOD(std::shared_ptr<IProcessMessage>, clone, (), (const, override));
    MOCK_METHOD(std::string, to_string, (), (const, override));
};

class MockHandler {
public:
    MOCK_METHOD(void, Call, (std::shared_ptr<IProcessMessage>), ());
};

// ProcessBase の終了フラグをテストから操作するための派生クラス
class ProcessBaseForTest : public ProcessBase {
public:
    using ProcessBase::ProcessBase;
    static void reset_stop_flag() { g_stop_flag.store(false); }
};

} // namespace

TEST(ProcessBaseIntegration, 正常系)
{
    mq_stub_reset();
    ProcessBaseForTest::reset_stop_flag();
    spdlog::drop_all();

    auto logger = std::make_shared<Logger>(spdlog::stdout_color_mt("process_base_normal"));

    auto codec = std::make_shared<StrictMock<MockMessageCodec>>();
    auto msg_mock   = std::make_shared<StrictMock<MockProcessMessage>>();
    std::shared_ptr<IProcessMessage> msg = msg_mock;

    auto queue = std::make_shared<ProcessQueue>(logger, codec, "/test_queue");

    StrictMock<MockHandler> handler_mock;
    std::promise<void> handler_called;
    auto handler_fn = [&](std::shared_ptr<IProcessMessage> m) {
        handler_mock.Call(m);
        handler_called.set_value();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    };

    ProcessDispatcher::HandlerMap map{
        {ProcessMessageType::HumanDetected, handler_fn}};
    auto dispatcher = std::make_shared<ProcessDispatcher>(logger, map);
    auto receiver   = std::make_shared<ProcessReceiver>(logger, queue, dispatcher);
    auto sender     = std::make_shared<ProcessSender>(queue, msg);

    // 設定ファイル作成
    char cfg_path[] = "/tmp/test_process_baseXXXXXX";
    int  fd        = ::mkstemp(cfg_path);
    ASSERT_NE(fd, -1);
    ::close(fd);
    {
        std::ofstream ofs(cfg_path);
        ofs << "priority=5\n";
    }

    auto file_loader = std::make_shared<FileLoader>(logger, cfg_path);

    std::vector<uint8_t> raw{'m', 's', 'g'};
    EXPECT_CALL(*codec, encode(msg)).WillOnce(Return(raw));
    EXPECT_CALL(*codec, decode(raw)).WillOnce(Return(msg));
    EXPECT_CALL(*msg_mock, type()).WillOnce(Return(ProcessMessageType::HumanDetected));
    EXPECT_CALL(handler_mock, Call(msg)).Times(1);

    ProcessBaseForTest process(queue, receiver, dispatcher, sender, file_loader, logger,
                               "test_process");

    sender->send();
    std::thread th([&] { EXPECT_EQ(process.run(), 0); });

    // ハンドラ実行を待ってから受信スレッドとメインループを停止
    handler_called.get_future().wait();
    receiver->stop();
    process.stop();
    th.join();

    EXPECT_EQ(process.priority(), 5);
    std::remove(cfg_path);
    ProcessBaseForTest::reset_stop_flag();
}

TEST(ProcessBaseIntegration, 異常系)
{
    mq_stub_reset();
    ProcessBaseForTest::reset_stop_flag();
    spdlog::drop_all();

    auto logger = std::make_shared<Logger>(spdlog::stdout_color_mt("process_base_error"));

    auto codec = std::make_shared<StrictMock<MockMessageCodec>>();
    auto msg_mock   = std::make_shared<StrictMock<MockProcessMessage>>();
    std::shared_ptr<IProcessMessage> msg = msg_mock;

    auto queue = std::make_shared<ProcessQueue>(logger, codec, "/test_queue_err");

    StrictMock<MockHandler> handler_mock;
    ProcessDispatcher::HandlerMap map{
        {ProcessMessageType::HumanDetected,
         [&](std::shared_ptr<IProcessMessage> m) { handler_mock.Call(m); }}};
    auto dispatcher = std::make_shared<ProcessDispatcher>(logger, map);
    auto receiver   = std::make_shared<ProcessReceiver>(logger, queue, dispatcher);
    auto sender     = std::make_shared<ProcessSender>(queue, msg);

    char cfg_path[] = "/tmp/test_process_base_errXXXXXX";
    int  fd        = ::mkstemp(cfg_path);
    ASSERT_NE(fd, -1);
    ::close(fd);
    {
        std::ofstream ofs(cfg_path);
        ofs << "priority=abc\n";
    }

    auto file_loader = std::make_shared<FileLoader>(logger, cfg_path);

    EXPECT_CALL(handler_mock, Call(_)).Times(0);

    EXPECT_THROW(
        ProcessBaseForTest(queue, receiver, dispatcher, sender, file_loader, logger,
                           "test_process"),
        std::exception);

    std::remove(cfg_path);
    ProcessBaseForTest::reset_stop_flag();
}
