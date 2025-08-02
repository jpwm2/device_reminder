#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "core/buzzer_task/buzzer_handler.hpp"
#include "core/buzzer_task/buzzer_task.hpp"
#include "infra/timer_service/timer_service.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/thread_operation/thread_sender/i_thread_sender.hpp"
#include "infra/buzzer_driver/i_buzzer_driver.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/logger/logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <thread>
#include <vector>

using namespace device_reminder;
using ::testing::StrictMock;

namespace {

class MockThreadSender : public IThreadSender {
public:
    MOCK_METHOD(void, send, (), (override));
};

class MockBuzzerDriver : public IBuzzerDriver {
public:
    MOCK_METHOD(void, on, (), (override));
    MOCK_METHOD(void, off, (), (override));
};

class MockProcessSender : public IProcessSender {
public:
    MOCK_METHOD(void, send, (), (override));
};

class MockFileLoader : public IFileLoader {
public:
    MOCK_METHOD(int, load_int, (const std::string&), (const, override));
    MOCK_METHOD(std::string, load_string, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<std::string>, load_string_list, (const std::string&), (const, override));
};

std::shared_ptr<ILogger> make_logger(const std::string& name) {
    auto base = spdlog::get(name);
    if (!base) {
        base = spdlog::stdout_color_mt(name);
    }
    base->set_level(spdlog::level::off);
    return std::make_shared<Logger>(base);
}

} // namespace

TEST(BuzzerHandlerIntegrationTest, StartBuzzingTriggersTimerAndStopsBuzzer) {
    auto logger       = make_logger("bh_start");
    auto driver       = std::make_shared<StrictMock<MockBuzzerDriver>>();
    auto proc_sender  = std::make_shared<StrictMock<MockProcessSender>>();
    auto file_loader  = std::make_shared<StrictMock<MockFileLoader>>();
    auto threadSender = std::make_shared<StrictMock<MockThreadSender>>();

    auto task  = std::make_shared<BuzzerTask>(logger, proc_sender, file_loader, driver);
    auto timer = std::make_shared<TimerService>(logger, 10, threadSender);
    BuzzerHandler handler(logger, task, timer);

    EXPECT_CALL(*driver, off()).Times(1);
    EXPECT_CALL(*threadSender, send()).Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{});
    handler.handle(msg);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

TEST(BuzzerHandlerIntegrationTest, HandleNullMessageDoesNothing) {
    auto logger       = make_logger("bh_null");
    auto driver       = std::make_shared<StrictMock<MockBuzzerDriver>>();
    auto proc_sender  = std::make_shared<StrictMock<MockProcessSender>>();
    auto file_loader  = std::make_shared<StrictMock<MockFileLoader>>();
    auto threadSender = std::make_shared<StrictMock<MockThreadSender>>();

    auto task  = std::make_shared<BuzzerTask>(logger, proc_sender, file_loader, driver);
    auto timer = std::make_shared<TimerService>(logger, 10, threadSender);
    BuzzerHandler handler(logger, task, timer);

    EXPECT_CALL(*driver, on()).Times(0);
    EXPECT_CALL(*driver, off()).Times(0);
    EXPECT_CALL(*threadSender, send()).Times(0);

    handler.handle(nullptr);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

