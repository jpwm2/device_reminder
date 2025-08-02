#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/thread_operation/thread_sender/thread_sender.hpp"
#include "infra/thread_operation/thread_queue/thread_queue.hpp"
#include "infra/thread_operation/thread_message/thread_message.hpp"
#include "infra/logger/logger.hpp"

#include <spdlog/logger.h>
#include <spdlog/sinks/sink.h>

using namespace device_reminder;
using ::testing::StrictMock;
using ::testing::Truly;
using ::testing::InSequence;

namespace {
class MockSink : public spdlog::sinks::sink {
public:
    MOCK_METHOD(void, log, (const spdlog::details::log_msg& msg), (override));
    MOCK_METHOD(void, flush, (), (override));
    MOCK_METHOD(void, set_pattern, (const std::string& pattern), (override));
    void set_formatter(std::unique_ptr<spdlog::formatter>) override {}
};
}

TEST(ThreadSenderIntegrationTest, SendPushesMessageToQueue) {
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    auto logger = std::make_shared<Logger>(spdlogger);
    auto queue = std::make_shared<ThreadQueue>(nullptr);
    auto message = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing, std::vector<std::string>{"1"});

    {
        InSequence seq;
        EXPECT_CALL(*sink, log(Truly([](const spdlog::details::log_msg& msg){
            return msg.level == spdlog::level::info && msg.payload == "ThreadSender created";
        }))).Times(1);
    }

    ThreadSender sender(logger, queue, message);
    sender.send();

    EXPECT_EQ(queue->size(), 1u);
    auto res = queue->pop();
    ASSERT_NE(res, nullptr);
    EXPECT_EQ(res->type(), message->type());
    EXPECT_EQ(res->payload(), message->payload());
}

TEST(ThreadSenderIntegrationTest, SendFailsWhenQueueIsNull) {
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    auto logger = std::make_shared<Logger>(spdlogger);
    auto message = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing, std::vector<std::string>{"1"});

    {
        InSequence seq;
        EXPECT_CALL(*sink, log(Truly([](const spdlog::details::log_msg& msg){
            return msg.level == spdlog::level::info && msg.payload == "ThreadSender created";
        })));
        EXPECT_CALL(*sink, log(Truly([](const spdlog::details::log_msg& msg){
            return msg.level == spdlog::level::err && msg.payload == "ThreadSender send failed: null queue or message";
        })));
    }

    ThreadSender sender(logger, nullptr, message);
    sender.send();
}

TEST(ThreadSenderIntegrationTest, SendFailsWhenMessageIsNull) {
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    auto logger = std::make_shared<Logger>(spdlogger);
    auto queue = std::make_shared<ThreadQueue>(nullptr);

    {
        InSequence seq;
        EXPECT_CALL(*sink, log(Truly([](const spdlog::details::log_msg& msg){
            return msg.level == spdlog::level::info && msg.payload == "ThreadSender created";
        })));
        EXPECT_CALL(*sink, log(Truly([](const spdlog::details::log_msg& msg){
            return msg.level == spdlog::level::err && msg.payload == "ThreadSender send failed: null queue or message";
        })));
    }

    ThreadSender sender(logger, queue, nullptr);
    sender.send();
    EXPECT_EQ(queue->size(), 0u);
}

