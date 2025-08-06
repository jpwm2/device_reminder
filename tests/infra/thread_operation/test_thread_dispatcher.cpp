#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/thread_operation/thread_dispatcher/thread_dispatcher.hpp"
#include "infra/message/message.hpp"
#include "infra/logger/i_logger.hpp"

using ::testing::NiceMock;
using ::testing::StartsWith;

using namespace device_reminder;

namespace {
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};
} // namespace

TEST(ThreadDispatcherTest, ThrowsOnNullMessage) {
    NiceMock<MockLogger> logger;
    ThreadDispatcher dispatcher(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), {});

    EXPECT_CALL(logger, error("msg is nullptr")).Times(1);
    EXPECT_THROW(dispatcher.dispatch(nullptr), std::invalid_argument);
}

TEST(ThreadDispatcherTest, WarnsOnUnknownType) {
    NiceMock<MockLogger> logger;
    ThreadDispatcher dispatcher(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), {});

    auto msg = std::make_shared<Message>(MessageType::StartBuzzer, std::vector<std::string>{},
                                         std::shared_ptr<ILogger>(&logger, [](ILogger*){}));
    std::string expected = "Unknown MessageType: " +
                           std::to_string(static_cast<int>(MessageType::StartBuzzer));
    EXPECT_CALL(logger, warn(expected)).Times(1);
    EXPECT_CALL(logger, info(::testing::_)).Times(0);

    dispatcher.dispatch(msg);
}

TEST(ThreadDispatcherTest, CallsHandlerAndLogsInfo) {
    NiceMock<MockLogger> logger;
    bool called = false;
    ThreadDispatcher::HandlerMap handlers{
        {MessageType::StartBuzzer, [&](std::shared_ptr<IMessage>) { called = true; }}
    };
    ThreadDispatcher dispatcher(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), handlers);

    auto msg = std::make_shared<Message>(MessageType::StartBuzzer, std::vector<std::string>{},
                                         std::shared_ptr<ILogger>(&logger, [](ILogger*){}));

    std::string expected = "Dispatch success for type: " +
                           std::to_string(static_cast<int>(MessageType::StartBuzzer));
    EXPECT_CALL(logger, info(expected)).Times(1);
    EXPECT_CALL(logger, warn(::testing::_)).Times(0);

    dispatcher.dispatch(msg);
    EXPECT_TRUE(called);
}

