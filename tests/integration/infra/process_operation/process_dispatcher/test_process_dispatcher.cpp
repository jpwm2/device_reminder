#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/process_operation/process_dispatcher/process_dispatcher.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/process_operation/process_message/process_message_type.hpp"
#include "infra/logger/logger.hpp"
#include "core/buzzer_task/buzzer_handler.hpp"
#include "core/buzzer_task/i_buzzer_task.hpp"
#include "infra/timer_service/i_timer_service.hpp"
#include "infra/thread_operation/thread_message/i_thread_message.hpp"

#include <spdlog/logger.h>
#include <spdlog/details/log_msg.h>

using ::testing::_;
using ::testing::NiceMock;
using ::testing::StrictMock;
using ::testing::Truly;

namespace {
class MockSpdLogger : public spdlog::logger {
public:
    MockSpdLogger() : spdlog::logger("mock") {}
    MOCK_METHOD(void, sink_it_, (const spdlog::details::log_msg& msg), (override));
};

class MockBuzzerTask : public device_reminder::IBuzzerTask {
public:
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(bool, send_message, (const device_reminder::IThreadMessage&), (override));
    MOCK_METHOD(void, on_waiting, (const std::vector<std::string>&), (override));
    MOCK_METHOD(void, on_buzzing, (const std::vector<std::string>&), (override));
};

class MockTimerService : public device_reminder::ITimerService {
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};
} // namespace

using namespace device_reminder;

TEST(ProcessDispatcherIntegrationTest, DispatchesValidMessage) {
    auto spd = std::make_shared<NiceMock<MockSpdLogger>>();
    auto logger = std::make_shared<Logger>(spd);

    auto task = std::make_shared<StrictMock<MockBuzzerTask>>();
    auto timer = std::make_shared<StrictMock<MockTimerService>>();
    auto handler = std::make_shared<BuzzerHandler>(logger, task, timer);

    ProcessDispatcher::HandlerMap handlers{
        {ProcessMessageType::StartBuzzing, [handler](std::shared_ptr<IProcessMessage> m) { handler->handle(m); }}
    };
    ProcessDispatcher dispatcher(logger, handlers);

    EXPECT_CALL(*timer, start()).Times(1);
    EXPECT_CALL(*task, on_buzzing(_)).Times(1);
    EXPECT_CALL(*spd, sink_it_(Truly([](const spdlog::details::log_msg& msg) {
                        return msg.level == spdlog::level::info &&
                               std::string(msg.payload.data(), msg.payload.size()) == "StartBuzzing";
                    })))
        .Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{});
    dispatcher.dispatch(msg);
}

TEST(ProcessDispatcherIntegrationTest, NullMessageLogsError) {
    auto spd = std::make_shared<NiceMock<MockSpdLogger>>();
    auto logger = std::make_shared<Logger>(spd);

    auto task = std::make_shared<StrictMock<MockBuzzerTask>>();
    auto timer = std::make_shared<StrictMock<MockTimerService>>();
    auto handler = std::make_shared<BuzzerHandler>(logger, task, timer);

    ProcessDispatcher::HandlerMap handlers{
        {ProcessMessageType::StartBuzzing, [handler](std::shared_ptr<IProcessMessage> m) { handler->handle(m); }}
    };
    ProcessDispatcher dispatcher(logger, handlers);

    EXPECT_CALL(*timer, start()).Times(0);
    EXPECT_CALL(*task, on_buzzing(_)).Times(0);
    EXPECT_CALL(*spd, sink_it_(Truly([](const spdlog::details::log_msg& msg) {
                        return msg.level == spdlog::level::err &&
                               std::string(msg.payload.data(), msg.payload.size()) == "Null process message";
                    })))
        .Times(1);

    dispatcher.dispatch(nullptr);
}

