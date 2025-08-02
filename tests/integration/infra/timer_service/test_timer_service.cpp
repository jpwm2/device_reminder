#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/timer_service/timer_service.hpp"
#include "infra/logger/logger.hpp"
#include "infra/thread_operation/thread_sender/thread_sender.hpp"
#include "infra/thread_operation/thread_queue/i_thread_queue.hpp"
#include "infra/thread_operation/thread_message/i_thread_message.hpp"

#include <spdlog/logger.h>
#include <spdlog/sinks/sink.h>

#include <chrono>
#include <thread>

using namespace device_reminder;
using ::testing::StrictMock;
using ::testing::_;
using ::testing::Truly;

class MockThreadQueue : public IThreadQueue {
public:
    MOCK_METHOD(void, push, (std::shared_ptr<IThreadMessage>), (override));
    MOCK_METHOD(std::shared_ptr<IThreadMessage>, pop, (), (override));
    MOCK_METHOD(size_t, size, (), (const, override));
};

class MockThreadMessage : public IThreadMessage {
public:
    MOCK_METHOD(ThreadMessageType, type, (), (const, noexcept, override));
    MOCK_METHOD(std::vector<std::string>, payload, (), (const, override));
    MOCK_METHOD(std::shared_ptr<IThreadMessage>, clone, (), (const, override));
    MOCK_METHOD(std::string, to_string, (), (const, override));
};

class MockSink : public spdlog::sinks::sink {
public:
    MOCK_METHOD(void, log, (const spdlog::details::log_msg&), (override));
    MOCK_METHOD(void, flush, (), (override));
    MOCK_METHOD(void, set_pattern, (const std::string&), (override));
    void set_formatter(std::unique_ptr<spdlog::formatter>) override {}
};

auto InfoMsgEq(const std::string& s) {
    return Truly([s](const spdlog::details::log_msg& m) {
        return m.level == spdlog::level::info && m.payload == s;
    });
}

auto ErrorMsgEq(const std::string& s) {
    return Truly([s](const spdlog::details::log_msg& m) {
        return m.level == spdlog::level::err && m.payload == s;
    });
}

// 正常系：タイムアウトでメッセージが送信される
TEST(統合テストTimerService, 正常系_タイムアウトで送信) {
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    auto logger = std::make_shared<Logger>(spdlogger);

    auto queue = std::make_shared<StrictMock<MockThreadQueue>>();
    auto msg = std::make_shared<StrictMock<MockThreadMessage>>();
    std::shared_ptr<IThreadMessage> msg_base = msg;

    {
        testing::InSequence seq;
        EXPECT_CALL(*sink, log(InfoMsgEq("ThreadSender created")));
        EXPECT_CALL(*sink, log(InfoMsgEq("TimerService created")));
        EXPECT_CALL(*sink, log(InfoMsgEq("TimerService stopped")));
        EXPECT_CALL(*sink, log(InfoMsgEq("TimerService started")));
        EXPECT_CALL(*queue, push(msg_base));
        EXPECT_CALL(*sink, log(InfoMsgEq("TimerService timeout")));
        EXPECT_CALL(*sink, log(InfoMsgEq("TimerService stopped")));
        EXPECT_CALL(*sink, log(InfoMsgEq("TimerService destroyed")));
    }

    auto sender = std::make_shared<ThreadSender>(logger, queue, msg_base);

    {
        TimerService timer(logger, 5, sender);
        timer.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// 異常系：メッセージ未設定の場合は送信されない
TEST(統合テストTimerService, 異常系_メッセージ未設定) {
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    auto logger = std::make_shared<Logger>(spdlogger);

    auto queue = std::make_shared<StrictMock<MockThreadQueue>>();

    {
        testing::InSequence seq;
        EXPECT_CALL(*sink, log(InfoMsgEq("ThreadSender created")));
        EXPECT_CALL(*sink, log(InfoMsgEq("TimerService created")));
        EXPECT_CALL(*sink, log(InfoMsgEq("TimerService stopped")));
        EXPECT_CALL(*sink, log(InfoMsgEq("TimerService started")));
        EXPECT_CALL(*sink, log(ErrorMsgEq("ThreadSender send failed: null queue or message")));
        EXPECT_CALL(*sink, log(InfoMsgEq("TimerService timeout")));
        EXPECT_CALL(*sink, log(InfoMsgEq("TimerService stopped")));
        EXPECT_CALL(*sink, log(InfoMsgEq("TimerService destroyed")));
    }
    EXPECT_CALL(*queue, push(_)).Times(0);

    auto sender = std::make_shared<ThreadSender>(logger, queue, nullptr);

    {
        TimerService timer(logger, 5, sender);
        timer.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

