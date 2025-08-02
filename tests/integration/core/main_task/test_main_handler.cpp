#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "main_task/main_handler.hpp"
#include "main_task/main_task.hpp"
#include "process_operation/process_message/process_message.hpp"
#include "process_operation/process_message/process_message_type.hpp"
#include "process_operation/process_sender/i_process_sender.hpp"
#include "timer_service/i_timer_service.hpp"
#include "file_loader/i_file_loader.hpp"
#include "logger/i_logger.hpp"

using ::testing::StrictMock;
using ::testing::NiceMock;

namespace device_reminder {

// ダミーロガー
class DummyLogger : public ILogger {
public:
    void info(const std::string&) override {}
    void error(const std::string&) override {}
    void warn(const std::string&) override {}
};

// 固定値を返すファイルローダ
class StubFileLoader : public IFileLoader {
public:
    int load_int(const std::string&) const override { return 0; }
    std::string load_string(const std::string&) const override { return ""; }
    std::vector<std::string> load_string_list(const std::string& key) const override {
        if (key == "device_list") {
            return {"device1"};
        }
        return {};
    }
};

// 何もしないタイマーサービス
class DummyTimerService : public ITimerService {
public:
    void start() override {}
    void stop() override {}
};

// 下位モジュールのモック
class MockProcessSender : public IProcessSender {
public:
    MOCK_METHOD(void, send, (), (override));
};

class MockTimerService : public ITimerService {
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};

} // namespace device_reminder

using namespace device_reminder;

// 正常系: 人を検知したら Bluetooth スキャン開始とタイマー起動
TEST(MainHandlerIntegration, HumanDetectedStartsScan) {
    auto logger     = std::make_shared<DummyLogger>();
    auto fileLoader = std::make_shared<StubFileLoader>();
    auto btSender   = std::make_shared<StrictMock<MockProcessSender>>();
    auto detTimer   = std::make_shared<StrictMock<MockTimerService>>();
    auto handlerTimer = std::make_shared<DummyTimerService>();

    auto task = std::make_shared<MainTask>(logger, fileLoader,
                                           nullptr, nullptr,
                                           btSender, nullptr, nullptr,
                                           detTimer, nullptr);
    MainHandler handler(logger, task, handlerTimer, fileLoader);

    EXPECT_CALL(*btSender, send()).Times(1);
    EXPECT_CALL(*detTimer, start()).Times(1);

    auto msg = std::make_shared<ProcessMessage>(
        ProcessMessageType::HumanDetected, std::vector<std::string>{});
    handler.handle(msg);

    EXPECT_EQ(task->state(), MainTask::State::WaitDeviceResponse);
}

// 正常系: 登録済みデバイスが見つかったら人検知再開とブザー停止
TEST(MainHandlerIntegration, DeviceFoundResumesHumanDetect) {
    auto logger      = std::make_shared<DummyLogger>();
    auto fileLoader  = std::make_shared<StubFileLoader>();
    auto humanSender = std::make_shared<StrictMock<MockProcessSender>>();
    auto buzzerStop  = std::make_shared<StrictMock<MockProcessSender>>();
    auto btSender    = std::make_shared<NiceMock<MockProcessSender>>();
    auto detTimer    = std::make_shared<StrictMock<MockTimerService>>();
    auto handlerTimer = std::make_shared<DummyTimerService>();

    auto task = std::make_shared<MainTask>(logger, fileLoader,
                                           humanSender, nullptr,
                                           btSender, nullptr, buzzerStop,
                                           detTimer, nullptr);
    MainHandler handler(logger, task, handlerTimer, fileLoader);

    // 初回の検知で Bluetooth スキャンとタイマー開始
    EXPECT_CALL(*btSender, send()).Times(1);
    EXPECT_CALL(*detTimer, start()).Times(1);

    auto detMsg = std::make_shared<ProcessMessage>(
        ProcessMessageType::HumanDetected, std::vector<std::string>{});
    handler.handle(detMsg);

    EXPECT_CALL(*humanSender, send()).Times(1);
    EXPECT_CALL(*buzzerStop, send()).Times(1);
    EXPECT_CALL(*detTimer, stop()).Times(1);

    auto msg = std::make_shared<ProcessMessage>(
        ProcessMessageType::ResponseDevicePresence,
        std::vector<std::string>{"found", "device1"});
    handler.handle(msg);

    EXPECT_EQ(task->state(), MainTask::State::WaitHumanDetect);
}

// 異常系: null メッセージでは何も起こらない
TEST(MainHandlerIntegration, NullMessageDoesNothing) {
    auto logger     = std::make_shared<DummyLogger>();
    auto fileLoader = std::make_shared<StubFileLoader>();
    auto btSender   = std::make_shared<StrictMock<MockProcessSender>>();
    auto detTimer   = std::make_shared<StrictMock<MockTimerService>>();
    auto handlerTimer = std::make_shared<DummyTimerService>();

    auto task = std::make_shared<MainTask>(logger, fileLoader,
                                           nullptr, nullptr,
                                           btSender, nullptr, nullptr,
                                           detTimer, nullptr);
    MainHandler handler(logger, task, handlerTimer, fileLoader);

    EXPECT_CALL(*btSender, send()).Times(0);
    EXPECT_CALL(*detTimer, start()).Times(0);

    handler.handle(nullptr);

    EXPECT_EQ(task->state(), MainTask::State::WaitHumanDetect);
}

