#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "app/app.hpp"
#include "setpriority_stub.h"

namespace device_reminder {

class MockProcess : public IProcess {
public:
    MOCK_METHOD(pid_t, spawn, (), (override));
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

} // namespace device_reminder

using namespace device_reminder;
using ::testing::Return;
using ::testing::StrictMock;
using ::testing::InSequence;
using ::testing::NiceMock;

TEST(AppTest, RunSpawnsAllProcessesAndSetsPriority) {
    auto main = std::make_unique<StrictMock<MockProcess>>();
    auto human = std::make_unique<StrictMock<MockProcess>>();
    auto bluetooth = std::make_unique<StrictMock<MockProcess>>();
    auto buzzer = std::make_unique<StrictMock<MockProcess>>();
    auto logger = std::make_unique<StrictMock<MockLogger>>();

    SetPriorityMock sp;
    g_setpriority_mock = &sp;

    {
        InSequence seq;
        EXPECT_CALL(*main, spawn()).WillOnce(Return(1));
        EXPECT_CALL(sp, call(PRIO_PROCESS, 1, 0)).WillOnce(Return(0));
        EXPECT_CALL(*human, spawn()).WillOnce(Return(2));
        EXPECT_CALL(sp, call(PRIO_PROCESS, 2, 0)).WillOnce(Return(0));
        EXPECT_CALL(*bluetooth, spawn()).WillOnce(Return(3));
        EXPECT_CALL(sp, call(PRIO_PROCESS, 3, 0)).WillOnce(Return(0));
        EXPECT_CALL(*buzzer, spawn()).WillOnce(Return(4));
        EXPECT_CALL(sp, call(PRIO_PROCESS, 4, 0)).WillOnce(Return(0));
    }
    EXPECT_CALL(*logger, info(testing::_)).Times(testing::AtLeast(1));

    App app(std::move(main), std::move(human), std::move(bluetooth), std::move(buzzer), std::move(logger));
    app.run();
    g_setpriority_mock = nullptr;
}

TEST(AppTest, RunPropagatesException) {
    auto main = std::make_unique<StrictMock<MockProcess>>();
    auto human = std::make_unique<StrictMock<MockProcess>>();
    auto bluetooth = std::make_unique<StrictMock<MockProcess>>();
    auto buzzer = std::make_unique<StrictMock<MockProcess>>();
    auto logger = std::make_unique<NiceMock<MockLogger>>();

    EXPECT_CALL(*main, spawn()).WillOnce(testing::Throw(std::runtime_error("fail")));
    EXPECT_CALL(*logger, error(testing::HasSubstr("fail")));

    App app(std::move(main), std::move(human), std::move(bluetooth), std::move(buzzer), std::move(logger));
    EXPECT_THROW(app.run(), std::runtime_error);
}
