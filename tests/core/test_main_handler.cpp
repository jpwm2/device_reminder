#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "main_task/main_handler.hpp"
#include "infra/process_operation/process_message/process_message.hpp"

using ::testing::StrictMock;

namespace device_reminder {

class MockMainTask : public IMainTask {
public:
    MOCK_METHOD(void, run, (const IThreadMessage&), (override));
    MOCK_METHOD(void, on_waiting_for_human, (const std::vector<std::string>&), (override));
    MOCK_METHOD(void, on_response_to_buzzer_task, (const std::vector<std::string>&), (override));
    MOCK_METHOD(void, on_response_to_human_task, (const std::vector<std::string>&), (override));
    MOCK_METHOD(void, on_cooldown, (const std::vector<std::string>&), (override));
    MOCK_METHOD(void, on_waiting_for_second_response, (const std::vector<std::string>&), (override));
};

class DummyLogger : public ILogger {
public:
    void info(const std::string&) override {}
    void error(const std::string&) override {}
    void warn(const std::string&) override {}
};

TEST(MainHandlerTest, HumanDetectedCallsTask) {
    auto task = std::make_shared<StrictMock<MockMainTask>>();
    auto logger = std::make_shared<DummyLogger>();
    MainHandler handler(logger, task, nullptr, nullptr);

    EXPECT_CALL(*task, on_waiting_for_human(testing::_)).Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::HumanDetected, std::vector<std::string>{});
    handler.handle(msg);
}

TEST(MainHandlerTest, DeviceFoundCallsHumanControl) {
    auto task = std::make_shared<StrictMock<MockMainTask>>();
    auto logger = std::make_shared<DummyLogger>();
    MainHandler handler(logger, task, nullptr, nullptr);

    EXPECT_CALL(*task, on_response_to_human_task(testing::_)).Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::ResponseDevicePresence, std::vector<std::string>{"found"});
    handler.handle(msg);
}

TEST(MainHandlerTest, DeviceNotFoundCallsBuzzerControl) {
    auto task = std::make_shared<StrictMock<MockMainTask>>();
    auto logger = std::make_shared<DummyLogger>();
    MainHandler handler(logger, task, nullptr, nullptr);

    EXPECT_CALL(*task, on_response_to_buzzer_task(testing::_)).Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::ResponseDevicePresence, std::vector<std::string>{"none"});
    handler.handle(msg);
}

TEST(MainHandlerTest, CooldownCallsTask) {
    auto task = std::make_shared<StrictMock<MockMainTask>>();
    auto logger = std::make_shared<DummyLogger>();
    MainHandler handler(logger, task, nullptr, nullptr);

    EXPECT_CALL(*task, on_cooldown(testing::_)).Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::CoolDownTimeout, std::vector<std::string>{});
    handler.handle(msg);
}

} // namespace device_reminder
