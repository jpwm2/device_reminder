#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "human_task/human_task.hpp"

using ::testing::StrictMock;
using ::testing::NiceMock;

namespace device_reminder {

class MockPIR : public IPIRDriver {
public:
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};

class MockSender : public IProcessSender {
public:
    MOCK_METHOD(void, send, (), (override));
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

TEST(HumanTaskTest, DetectingStartsDriver) {
    auto pir = std::make_shared<StrictMock<MockPIR>>();
    auto sender = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<DummyLogger>();
    HumanTask task(logger, pir, sender);

    EXPECT_CALL(*pir, run()).Times(1);
    EXPECT_CALL(*sender, send()).Times(0);

    task.on_detecting({});
}

TEST(HumanTaskTest, StoppingDoesNotSend) {
    auto pir = std::make_shared<StrictMock<MockPIR>>();
    auto sender = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<DummyLogger>();
    HumanTask task(logger, pir, sender);

    EXPECT_CALL(*sender, send()).Times(0);
    EXPECT_CALL(*pir, stop()).Times(1);

    task.on_stopping({});
}

TEST(HumanTaskTest, CooldownDoesNothing) {
    auto pir = std::make_shared<StrictMock<MockPIR>>();
    auto sender = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<DummyLogger>();
    HumanTask task(logger, pir, sender);

    EXPECT_CALL(*sender, send()).Times(0);
    EXPECT_CALL(*pir, stop()).Times(1);

    task.on_cooldown({});
}

// --- Additional tests for pointer and logger behaviours ---

TEST(HumanTaskTest, ConstructorLogsWhenLoggerProvided) {
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    EXPECT_CALL(*logger, info(testing::_)).Times(1);
    HumanTask task(logger, nullptr, nullptr);
}

TEST(HumanTaskTest, ConstructorNoLogWhenLoggerNull) {
    EXPECT_NO_THROW(HumanTask(nullptr, nullptr, nullptr));
}

TEST(HumanTaskTest, DetectingRunsAndLogs) {
    auto pir = std::make_shared<StrictMock<MockPIR>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    HumanTask task(logger, pir, nullptr);

    EXPECT_CALL(*pir, run()).Times(1);
    EXPECT_CALL(*logger, info(testing::_)).Times(1);

    task.on_detecting({"a"});
}

TEST(HumanTaskTest, DetectingWithoutPirOnlyLogs) {
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    HumanTask task(logger, nullptr, nullptr);

    EXPECT_CALL(*logger, info(testing::_)).Times(1);

    task.on_detecting({});
}

TEST(HumanTaskTest, DetectingWithoutLoggerOnlyRuns) {
    auto pir = std::make_shared<StrictMock<MockPIR>>();
    HumanTask task(nullptr, pir, nullptr);

    EXPECT_CALL(*pir, run()).Times(1);

    task.on_detecting({});
}

TEST(HumanTaskTest, StoppingStopsAndLogs) {
    auto pir = std::make_shared<StrictMock<MockPIR>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    HumanTask task(logger, pir, nullptr);

    EXPECT_CALL(*pir, stop()).Times(1);
    EXPECT_CALL(*logger, info(testing::_)).Times(1);

    task.on_stopping({"b"});
}

TEST(HumanTaskTest, StoppingWithoutPirOnlyLogs) {
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    HumanTask task(logger, nullptr, nullptr);

    EXPECT_CALL(*logger, info(testing::_)).Times(1);

    task.on_stopping({});
}

TEST(HumanTaskTest, StoppingWithoutLoggerOnlyStops) {
    auto pir = std::make_shared<StrictMock<MockPIR>>();
    HumanTask task(nullptr, pir, nullptr);

    EXPECT_CALL(*pir, stop()).Times(1);

    task.on_stopping({});
}

TEST(HumanTaskTest, CooldownStopsAndLogs) {
    auto pir = std::make_shared<StrictMock<MockPIR>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    HumanTask task(logger, pir, nullptr);

    EXPECT_CALL(*pir, stop()).Times(1);
    EXPECT_CALL(*logger, info(testing::_)).Times(1);

    task.on_cooldown({});
}

TEST(HumanTaskTest, CooldownWithoutPirOnlyLogs) {
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    HumanTask task(logger, nullptr, nullptr);

    EXPECT_CALL(*logger, info(testing::_)).Times(1);

    task.on_cooldown({});
}

TEST(HumanTaskTest, CooldownWithoutLoggerOnlyStops) {
    auto pir = std::make_shared<StrictMock<MockPIR>>();
    HumanTask task(nullptr, pir, nullptr);

    EXPECT_CALL(*pir, stop()).Times(1);

    task.on_cooldown({});
}

} // namespace device_reminder
