#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "human_task/human_task.hpp"

using ::testing::StrictMock;
using ::testing::NiceMock;

namespace device_reminder {

class MockPIR : public IPIRDriver {
public:
    MOCK_METHOD(int, read, (), (override));
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

TEST(HumanTaskTest, DetectingSendsMessageOnDetection) {
    auto pir = std::make_shared<StrictMock<MockPIR>>();
    auto sender = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<DummyLogger>();
    HumanTask task(logger, pir, sender);

    EXPECT_CALL(*pir, read()).WillOnce(::testing::Return(1));
    EXPECT_CALL(*sender, send()).Times(1);

    task.on_detecting({});
}

TEST(HumanTaskTest, StoppingDoesNotSend) {
    auto pir = std::make_shared<StrictMock<MockPIR>>();
    auto sender = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<DummyLogger>();
    HumanTask task(logger, pir, sender);

    EXPECT_CALL(*sender, send()).Times(0);
    EXPECT_CALL(*pir, read()).Times(0);

    task.on_stopping({});
}

TEST(HumanTaskTest, CooldownDoesNothing) {
    auto pir = std::make_shared<StrictMock<MockPIR>>();
    auto sender = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<DummyLogger>();
    HumanTask task(logger, pir, sender);

    EXPECT_CALL(*sender, send()).Times(0);
    EXPECT_CALL(*pir, read()).Times(0);

    task.on_cooldown({});
}

} // namespace device_reminder
