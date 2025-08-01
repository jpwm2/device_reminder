#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/thread_operation/thread_dispatcher/thread_dispatcher.hpp"
#include "infra/thread_operation/thread_message/thread_message.hpp"
#include "infra/logger/i_logger.hpp"

using ::testing::NiceMock;

using namespace device_reminder;

namespace {
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};
} // namespace

TEST(ThreadDispatcherTest, CallsRegisteredHandler) {
    bool called = false;
    ThreadDispatcher::HandlerMap map{
        {ThreadMessageType::StartBuzzing,
         [&](std::shared_ptr<IThreadMessage>) { called = true; }}
    };
    ThreadDispatcher disp(nullptr, map);
    auto msg = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing,
                                               std::vector<std::string>{});
    disp.dispatch(msg);
    EXPECT_TRUE(called);
}

TEST(ThreadDispatcherTest, IgnoresUnknownMessage) {
    bool called = false;
    ThreadDispatcher::HandlerMap map{
        {ThreadMessageType::StartBuzzing,
         [&](std::shared_ptr<IThreadMessage>) { called = true; }}
    };
    ThreadDispatcher disp(nullptr, map);
    auto msg = std::make_shared<ThreadMessage>(ThreadMessageType::StopBuzzing,
                                               std::vector<std::string>{});
    disp.dispatch(msg);
    EXPECT_FALSE(called);
}

TEST(ThreadDispatcherTest, ConstructorLogsCreation) {
    NiceMock<MockLogger> logger;
    EXPECT_CALL(logger, info("ThreadDispatcher created")).Times(1);

    ThreadDispatcher::HandlerMap map{};
    ThreadDispatcher disp(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), map);
}

TEST(ThreadDispatcherTest, DispatchLogsErrorOnNullMessage) {
    NiceMock<MockLogger> logger;
    EXPECT_CALL(logger, error("Null thread message")).Times(1);

    ThreadDispatcher::HandlerMap map{};
    ThreadDispatcher disp(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), map);
    disp.dispatch(nullptr);
}

TEST(ThreadDispatcherTest, DispatchLogsInfoOnUnhandledMessage) {
    NiceMock<MockLogger> logger;
    EXPECT_CALL(logger, info("Unhandled thread message")).Times(1);

    ThreadDispatcher::HandlerMap map{};
    ThreadDispatcher disp(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), map);
    auto msg = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing,
                                               std::vector<std::string>{});
    disp.dispatch(msg);
}
