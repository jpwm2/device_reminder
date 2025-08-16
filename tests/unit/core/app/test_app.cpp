#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "app/app.hpp"
#include "setpriority_stub.h"

using ::testing::Return;
using ::testing::StrictMock;
using ::testing::InSequence;

namespace device_reminder {

class MockProcess : public IProcess {
public:
    MOCK_METHOD(pid_t, spawn, (), (override));
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string& message), (override));
    MOCK_METHOD(void, error, (const std::string& message), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

} // namespace device_reminder

using namespace device_reminder;

TEST(AppTest, SpawnAndSetPriorityInOrder) {
    auto main = std::make_unique<StrictMock<MockProcess>>();
    auto human = std::make_unique<StrictMock<MockProcess>>();
    auto bluetooth = std::make_unique<StrictMock<MockProcess>>();
    auto buzzer = std::make_unique<StrictMock<MockProcess>>();
    auto logger = std::make_unique<StrictMock<MockLogger>>();

    auto* main_ptr = main.get();
    auto* human_ptr = human.get();
    auto* bluetooth_ptr = bluetooth.get();
    auto* buzzer_ptr = buzzer.get();
    auto* logger_ptr = logger.get();

    SetPriorityMock sp_mock;
    g_setpriority_mock = &sp_mock;

    {
        InSequence seq;
        EXPECT_CALL(*main_ptr, spawn()).WillOnce(Return(1));
        EXPECT_CALL(sp_mock, call(PRIO_PROCESS, 1, 0)).WillOnce(Return(0));
        EXPECT_CALL(*human_ptr, spawn()).WillOnce(Return(2));
        EXPECT_CALL(sp_mock, call(PRIO_PROCESS, 2, 0)).WillOnce(Return(0));
        EXPECT_CALL(*bluetooth_ptr, spawn()).WillOnce(Return(3));
        EXPECT_CALL(sp_mock, call(PRIO_PROCESS, 3, 0)).WillOnce(Return(0));
        EXPECT_CALL(*buzzer_ptr, spawn()).WillOnce(Return(4));
        EXPECT_CALL(sp_mock, call(PRIO_PROCESS, 4, 0)).WillOnce(Return(0));
    }
    EXPECT_CALL(*logger_ptr, info(testing::_)).Times(testing::AtLeast(1));

    App app(std::move(main), std::move(human), std::move(bluetooth), std::move(buzzer), std::move(logger));
    app.run();

    g_setpriority_mock = nullptr;
}
