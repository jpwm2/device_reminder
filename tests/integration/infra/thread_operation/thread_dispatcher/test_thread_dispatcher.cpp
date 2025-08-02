#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/thread_operation/thread_dispatcher/thread_dispatcher.hpp"
#include "infra/thread_operation/thread_sender/thread_sender.hpp"
#include "infra/thread_operation/thread_message/thread_message.hpp"
#include "infra/thread_operation/thread_queue/i_thread_queue.hpp"
#include "infra/logger/logger.hpp"
#include "infra/logger/i_logger.hpp"
#include <spdlog/sinks/ringbuffer_sink.h>

using namespace testing;
using namespace device_reminder;

namespace {
class MockThreadQueue : public IThreadQueue {
public:
    MOCK_METHOD(void, push, (std::shared_ptr<IThreadMessage>), (override));
    MOCK_METHOD(std::shared_ptr<IThreadMessage>, pop, (), (override));
    MOCK_METHOD(size_t, size, (), (const, override));
};

class MockSenderLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};
} // namespace

TEST(ThreadDispatcherIntegrationTest, DispatchValidMessageCallsHandler) {
    StrictMock<MockThreadQueue> queue;
    StrictMock<MockSenderLogger> sender_logger;
    EXPECT_CALL(sender_logger, info("ThreadSender created")).Times(1);
    std::shared_ptr<IThreadMessage> send_msg =
        std::make_shared<ThreadMessage>(
            ThreadMessageType::StartBuzzing,
            std::vector<std::string>{"payload"});
    ThreadSender sender(std::shared_ptr<ILogger>(&sender_logger, [](auto*){}),
                        std::shared_ptr<IThreadQueue>(&queue, [](auto*){}),
                        send_msg);

    ThreadDispatcher::HandlerMap handlers{
        {ThreadMessageType::StartBuzzing,
         [&sender](std::shared_ptr<IThreadMessage>) { sender.send(); }}
    };

    auto sink = std::make_shared<spdlog::sinks::ringbuffer_sink_mt>(10);
    auto spd_logger = std::make_shared<spdlog::logger>("test", sink);
    auto logger = std::make_shared<Logger>(spd_logger);

    ThreadDispatcher dispatcher(logger, handlers);

    EXPECT_CALL(queue, push(send_msg)).Times(1);

    auto dispatch_msg = std::make_shared<ThreadMessage>(
        ThreadMessageType::StartBuzzing, std::vector<std::string>{});
    dispatcher.dispatch(dispatch_msg);
}

TEST(ThreadDispatcherIntegrationTest, DispatchNullMessageLogsErrorAndSkipsHandler) {
    StrictMock<MockThreadQueue> queue;
    StrictMock<MockSenderLogger> sender_logger;
    EXPECT_CALL(sender_logger, info("ThreadSender created")).Times(1);
    std::shared_ptr<IThreadMessage> send_msg =
        std::make_shared<ThreadMessage>(
            ThreadMessageType::StartBuzzing,
            std::vector<std::string>{"payload"});
    ThreadSender sender(std::shared_ptr<ILogger>(&sender_logger, [](auto*){}),
                        std::shared_ptr<IThreadQueue>(&queue, [](auto*){}),
                        send_msg);

    ThreadDispatcher::HandlerMap handlers{
        {ThreadMessageType::StartBuzzing,
         [&sender](std::shared_ptr<IThreadMessage>) { sender.send(); }}
    };

    auto sink = std::make_shared<spdlog::sinks::ringbuffer_sink_mt>(10);
    auto spd_logger = std::make_shared<spdlog::logger>("test", sink);
    auto logger = std::make_shared<Logger>(spd_logger);

    ThreadDispatcher dispatcher(logger, handlers);

    EXPECT_CALL(queue, push).Times(0);

    dispatcher.dispatch(nullptr);

    const auto logs = sink->last_formatted();
    EXPECT_THAT(logs, Contains(HasSubstr("Null thread message")));
}

