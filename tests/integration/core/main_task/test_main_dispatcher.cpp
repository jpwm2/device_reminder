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

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::CooldownTimeout, std::vector<std::string>{});
    handler.handle(msg);
}

} // namespace device_reminder

namespace device_reminder {

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

TEST(MainHandlerExtendedTest, ConstructorLogsWhenLoggerProvided) {
    auto task = std::make_shared<StrictMock<MockMainTask>>();
    auto logger = std::make_shared<StrictMock<MockLogger>>();

    EXPECT_CALL(*logger, info(testing::StrEq("MainHandler created"))).Times(1);

    MainHandler handler(logger, task, nullptr, nullptr);
    (void)handler;
}

TEST(MainHandlerExtendedTest, ConstructorAcceptsNullLogger) {
    auto task = std::make_shared<StrictMock<MockMainTask>>();
    MainHandler handler(nullptr, task, nullptr, nullptr);
    (void)handler;
}

TEST(MainHandlerExtendedTest, ScanTimeoutCallsWaitingSecondResponse) {
    auto task = std::make_shared<StrictMock<MockMainTask>>();
    auto logger = std::make_shared<DummyLogger>();
    MainHandler handler(logger, task, nullptr, nullptr);

    EXPECT_CALL(*task, on_waiting_for_second_response(testing::_)).Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::ScanTimeout,
                                                std::vector<std::string>{});
    handler.handle(msg);
}

TEST(MainHandlerExtendedTest, InvalidPayloadCallsBuzzerTask) {
    auto task = std::make_shared<StrictMock<MockMainTask>>();
    auto logger = std::make_shared<DummyLogger>();
    MainHandler handler(logger, task, nullptr, nullptr);

    EXPECT_CALL(*task, on_response_to_buzzer_task(testing::_)).Times(1);

    auto msg = std::make_shared<ProcessMessage>(
        ProcessMessageType::ResponseDevicePresence,
        std::vector<std::string>{"invalid"});
    handler.handle(msg);
}

TEST(MainHandlerExtendedTest, UnknownMessageTypeDoesNothing) {
    auto task = std::make_shared<StrictMock<MockMainTask>>();
    auto logger = std::make_shared<DummyLogger>();
    MainHandler handler(logger, task, nullptr, nullptr);

    EXPECT_CALL(*task, on_waiting_for_human(testing::_)).Times(0);
    EXPECT_CALL(*task, on_response_to_buzzer_task(testing::_)).Times(0);
    EXPECT_CALL(*task, on_response_to_human_task(testing::_)).Times(0);
    EXPECT_CALL(*task, on_cooldown(testing::_)).Times(0);
    EXPECT_CALL(*task, on_waiting_for_second_response(testing::_)).Times(0);

    auto msg = std::make_shared<ProcessMessage>(
        static_cast<ProcessMessageType>(999), std::vector<std::string>{});
    handler.handle(msg);
}

TEST(MainHandlerExtendedTest, NullMessageDoesNothing) {
    auto task = std::make_shared<StrictMock<MockMainTask>>();
    auto logger = std::make_shared<DummyLogger>();
    MainHandler handler(logger, task, nullptr, nullptr);

    EXPECT_CALL(*task, on_waiting_for_human(testing::_)).Times(0);
    EXPECT_CALL(*task, on_response_to_buzzer_task(testing::_)).Times(0);
    EXPECT_CALL(*task, on_response_to_human_task(testing::_)).Times(0);
    EXPECT_CALL(*task, on_cooldown(testing::_)).Times(0);
    EXPECT_CALL(*task, on_waiting_for_second_response(testing::_)).Times(0);

    handler.handle(nullptr);
}

TEST(MainHandlerExtendedTest, NullTaskCausesNoCall) {
    auto dummy_task = std::make_shared<StrictMock<MockMainTask>>();
    auto logger = std::make_shared<DummyLogger>();
    MainHandler handler(logger, nullptr, nullptr, nullptr);

    EXPECT_CALL(*dummy_task, on_waiting_for_human(testing::_)).Times(0);
    EXPECT_CALL(*dummy_task, on_response_to_buzzer_task(testing::_)).Times(0);
    EXPECT_CALL(*dummy_task, on_response_to_human_task(testing::_)).Times(0);
    EXPECT_CALL(*dummy_task, on_cooldown(testing::_)).Times(0);
    EXPECT_CALL(*dummy_task, on_waiting_for_second_response(testing::_)).Times(0);

    auto msg = std::make_shared<ProcessMessage>(
        ProcessMessageType::HumanDetected, std::vector<std::string>{});
    handler.handle(msg);
}

} // namespace device_reminder
