#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/thread_operation/thread_dispatcher/thread_dispatcher.hpp"
#include "infra/thread_operation/thread_message/thread_message.hpp"
#include "infra/logger/i_logger.hpp"

using namespace device_reminder;
using ::testing::NiceMock;
using ::testing::StrictMock;

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
    ThreadDispatcher::HandlerMap map{{ThreadMessageType::StartBuzzing,
                                      [&](std::shared_ptr<IThreadMessage>) { called = true; }}};
    ThreadDispatcher disp(nullptr, map);
    auto msg = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing,
                                               std::vector<std::string>{});
    disp.dispatch(msg);
    EXPECT_TRUE(called);
}

TEST(ThreadDispatcherTest, LogsInfoForUnknownMessage) {
    NiceMock<MockLogger> logger;
    ThreadDispatcher::HandlerMap map{{ThreadMessageType::StartBuzzing,
                                      [&](std::shared_ptr<IThreadMessage>) {}}};
    {
        testing::InSequence seq;
        EXPECT_CALL(logger, info("ThreadDispatcher created"));
        EXPECT_CALL(logger, info("Unhandled thread message"));
    }
    ThreadDispatcher disp(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), map);
    auto msg = std::make_shared<ThreadMessage>(ThreadMessageType::StopBuzzing,
                                               std::vector<std::string>{});
    disp.dispatch(msg);
}

TEST(ThreadDispatcherTest, LogsErrorForNullMessage) {
    NiceMock<MockLogger> logger;
    ThreadDispatcher disp(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), {});
    EXPECT_CALL(logger, error("Null thread message")).Times(1);
    disp.dispatch(nullptr);
}

TEST(ThreadDispatcherTest, ConstructorLogsCreation) {
    StrictMock<MockLogger> logger;
    EXPECT_CALL(logger, info("ThreadDispatcher created")).Times(1);
    ThreadDispatcher disp(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), {});
}

TEST(ThreadDispatcherTest, ConstructorWithoutLoggerNoCrash) {
    ThreadDispatcher disp(nullptr, {});
    auto msg = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing,
                                               std::vector<std::string>{});
    EXPECT_NO_THROW(disp.dispatch(msg));
}

TEST(ThreadDispatcherTest, ConstructorPropagatesLoggerException) {
    class ThrowLogger : public ILogger {
    public:
        void info(const std::string&) override { throw std::runtime_error("boom"); }
        void error(const std::string&) override {}
        void warn(const std::string&) override {}
    };
    auto logger = std::make_shared<ThrowLogger>();
    EXPECT_THROW({ ThreadDispatcher disp(logger, {}); }, std::runtime_error);
}
