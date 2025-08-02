#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/sinks/null_sink.h>
#include <chrono>
#include <thread>

#include "human_task/human_handler.hpp"
#include "human_task/human_task.hpp"
#include "infra/logger/logger.hpp"
#include "infra/pir_driver/i_pir_driver.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/process_operation/process_message/process_message_type.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include "infra/timer_service/timer_service.hpp"
#include "infra/thread_operation/thread_sender/i_thread_sender.hpp"

using ::testing::StrictMock;

namespace device_reminder {

class MockPIRDriver : public IPIRDriver {
public:
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};

class MockProcessSender : public IProcessSender {
public:
    MOCK_METHOD(void, send, (), (override));
};

class MockThreadSender : public IThreadSender {
public:
    MOCK_METHOD(void, send, (), (override));
};

} // namespace device_reminder

TEST(HumanHandlerIntegrationTest, StopHumanDetectionTriggersPIRStopAndThreadSend) {
    using namespace device_reminder;

    auto sink = std::make_shared<spdlog::sinks::null_sink_mt>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    auto logger = std::make_shared<Logger>(spdlogger);

    auto pir = std::make_shared<StrictMock<MockPIRDriver>>();
    auto process_sender = std::make_shared<StrictMock<MockProcessSender>>();
    auto task = std::make_shared<HumanTask>(logger, pir, process_sender);

    auto thread_sender = std::make_shared<StrictMock<MockThreadSender>>();
    auto timer = std::make_shared<TimerService>(logger, 10, thread_sender);

    HumanHandler handler(logger, task, timer);

    EXPECT_CALL(*pir, stop()).Times(1);
    EXPECT_CALL(*thread_sender, send()).Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StopHumanDetection, std::vector<std::string>{});
    handler.handle(msg);

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    timer->stop();
}

TEST(HumanHandlerIntegrationTest, HandleNullMessageDoesNothing) {
    using namespace device_reminder;

    auto sink = std::make_shared<spdlog::sinks::null_sink_mt>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    auto logger = std::make_shared<Logger>(spdlogger);

    auto pir = std::make_shared<StrictMock<MockPIRDriver>>();
    auto process_sender = std::make_shared<StrictMock<MockProcessSender>>();
    auto task = std::make_shared<HumanTask>(logger, pir, process_sender);

    auto thread_sender = std::make_shared<StrictMock<MockThreadSender>>();
    auto timer = std::make_shared<TimerService>(logger, 10, thread_sender);

    HumanHandler handler(logger, task, timer);

    EXPECT_CALL(*pir, run()).Times(0);
    EXPECT_CALL(*pir, stop()).Times(0);
    EXPECT_CALL(*thread_sender, send()).Times(0);

    handler.handle(nullptr);

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    timer->stop();
}

