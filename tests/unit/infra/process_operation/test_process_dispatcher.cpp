#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/process_operation/process_dispatcher/process_dispatcher.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/logger/i_logger.hpp"

using ::testing::NiceMock;
using ::testing::StrictMock;

namespace {
class MockLogger : public device_reminder::ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};
} // namespace

using namespace device_reminder;

TEST(ProcessDispatcherTest, CallsRegisteredHandler) {
    bool called = false;
    ProcessDispatcher::HandlerMap map{
        {ProcessMessageType::StartBuzzing,
         [&](std::shared_ptr<IProcessMessage>) { called = true; }}
    };
    ProcessDispatcher disp(nullptr, map);
    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing,
                                               std::vector<std::string>{});
    disp.dispatch(msg);
    EXPECT_TRUE(called);
}

TEST(ProcessDispatcherTest, IgnoresUnknownMessage) {
    bool called = false;
    ProcessDispatcher::HandlerMap map{
        {ProcessMessageType::StartBuzzing,
         [&](std::shared_ptr<IProcessMessage>) { called = true; }}
    };
    ProcessDispatcher disp(nullptr, map);
    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StopBuzzing,
                                               std::vector<std::string>{});
    disp.dispatch(msg);
    EXPECT_FALSE(called);
}

TEST(ProcessDispatcherTest, LogsWarnForUnknownMessageWithType) {
    NiceMock<MockLogger> logger;
    ProcessDispatcher::HandlerMap map{{ProcessMessageType::StartBuzzing,
                                       [](std::shared_ptr<IProcessMessage>) {}}};
    ProcessDispatcher disp(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), map);
    auto msg =
        std::make_shared<ProcessMessage>(ProcessMessageType::StopBuzzing,
                                         std::vector<std::string>{});
    std::string expected =
        "Unhandled process message type " +
        std::to_string(static_cast<int>(ProcessMessageType::StopBuzzing));
    EXPECT_CALL(logger, warn(expected)).Times(1);
    disp.dispatch(msg);
}

TEST(ProcessDispatcherTest, LogsErrorWhenHandlerEmpty) {
    NiceMock<MockLogger> logger;
    ProcessDispatcher::HandlerMap map{{ProcessMessageType::StartBuzzing, {}}};
    ProcessDispatcher disp(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), map);
    auto msg =
        std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing,
                                         std::vector<std::string>{});
    EXPECT_CALL(logger, error("Empty process handler")).Times(1);
    disp.dispatch(msg);
}

