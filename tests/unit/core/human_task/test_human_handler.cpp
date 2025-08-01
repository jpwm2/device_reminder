#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "human_task/human_handler.hpp"
#include "infra/process_operation/process_message/process_message.hpp"

using ::testing::StrictMock;
using ::testing::NiceMock;

namespace device_reminder {

class MockHumanTask : public IHumanTask {
public:
    MOCK_METHOD(void, on_detecting, (const std::vector<std::string>&), (override));
    MOCK_METHOD(void, on_stopping, (const std::vector<std::string>&), (override));
    MOCK_METHOD(void, on_cooldown, (const std::vector<std::string>&), (override));
};

class MockTimer : public ITimerService {
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};

class DummyLogger : public ILogger {
public:
    void info(const std::string&) override {}
    void error(const std::string&) override {}
    void warn(const std::string&) override {}
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

TEST(HumanHandlerTest, StopDetectionStartsTimer) {
    auto task = std::make_shared<StrictMock<MockHumanTask>>();
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    auto logger = std::make_shared<DummyLogger>();
    HumanHandler handler(logger, task, timer);

    EXPECT_CALL(*task, on_stopping(testing::_)).Times(1);
    EXPECT_CALL(*timer, start()).Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StopHumanDetection, std::vector<std::string>{});
    handler.handle(msg);
}

TEST(HumanHandlerTest, StartDetectionStopsTimer) {
    auto task = std::make_shared<StrictMock<MockHumanTask>>();
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    auto logger = std::make_shared<DummyLogger>();
    HumanHandler handler(logger, task, timer);

    EXPECT_CALL(*task, on_detecting(testing::_)).Times(1);
    EXPECT_CALL(*timer, stop()).Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartHumanDetection, std::vector<std::string>{});
    handler.handle(msg);
}

TEST(HumanHandlerTest, CooldownCallsTask) {
    auto task = std::make_shared<StrictMock<MockHumanTask>>();
    auto timer = std::make_shared<NiceMock<MockTimer>>();
    auto logger = std::make_shared<DummyLogger>();
    HumanHandler handler(logger, task, timer);

    EXPECT_CALL(*task, on_cooldown(testing::_)).Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::CooldownTimeout, std::vector<std::string>{});
    handler.handle(msg);
}

TEST(HumanHandlerTest, ConstructorLogsCreation) {
    auto task = std::make_shared<NiceMock<MockHumanTask>>();
    auto timer = std::make_shared<NiceMock<MockTimer>>();
    auto logger = std::make_shared<StrictMock<MockLogger>>();

    EXPECT_CALL(*logger, info("HumanHandler created")).Times(1);

    HumanHandler handler(logger, task, timer);
}

TEST(HumanHandlerTest, ConstructorNullLoggerDoesNotThrow) {
    auto task = std::make_shared<NiceMock<MockHumanTask>>();
    auto timer = std::make_shared<NiceMock<MockTimer>>();

    EXPECT_NO_THROW({ HumanHandler handler(nullptr, task, timer); });
}

TEST(HumanHandlerTest, HandleNullMessageDoesNothing) {
    auto task = std::make_shared<StrictMock<MockHumanTask>>();
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    auto logger = std::make_shared<DummyLogger>();
    HumanHandler handler(logger, task, timer);

    EXPECT_CALL(*timer, start()).Times(0);
    EXPECT_CALL(*timer, stop()).Times(0);

    handler.handle(nullptr);
}

TEST(HumanHandlerTest, HandleWithNullTaskDoesNothing) {
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    auto logger = std::make_shared<DummyLogger>();
    HumanHandler handler(logger, nullptr, timer);

    EXPECT_CALL(*timer, start()).Times(0);
    EXPECT_CALL(*timer, stop()).Times(0);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartHumanDetection, std::vector<std::string>{});
    handler.handle(msg);
}

TEST(HumanHandlerTest, StartDetectionWithNullTimerOnlyCallsTask) {
    auto task = std::make_shared<StrictMock<MockHumanTask>>();
    auto logger = std::make_shared<DummyLogger>();
    HumanHandler handler(logger, task, nullptr);

    EXPECT_CALL(*task, on_detecting(testing::_)).Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartHumanDetection, std::vector<std::string>{});
    handler.handle(msg);
}

TEST(HumanHandlerTest, StopDetectionWithNullLoggerWorks) {
    auto task = std::make_shared<StrictMock<MockHumanTask>>();
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    HumanHandler handler(nullptr, task, timer);

    EXPECT_CALL(*timer, start()).Times(1);
    EXPECT_CALL(*task, on_stopping(testing::_)).Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StopHumanDetection, std::vector<std::string>{});
    handler.handle(msg);
}

TEST(HumanHandlerTest, UnknownMessageKeepsDetectingState) {
    auto task = std::make_shared<StrictMock<MockHumanTask>>();
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    auto logger = std::make_shared<DummyLogger>();
    HumanHandler handler(logger, task, timer);

    EXPECT_CALL(*timer, start()).Times(0);
    EXPECT_CALL(*timer, stop()).Times(0);
    EXPECT_CALL(*task, on_detecting(testing::_)).Times(1);

    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{});
    handler.handle(msg);
}

} // namespace device_reminder
