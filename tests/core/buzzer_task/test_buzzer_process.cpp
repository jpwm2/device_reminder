#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace device_reminder {}
using namespace device_reminder;
#include "buzzer_task/buzzer_process.hpp"

using ::testing::StrictMock;

namespace device_reminder {

class MockWatchDog : public IWatchDog {
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(void, kick, (), (override));
};

class DummyLogger : public ILogger {
public:
    void info(const std::string&) override {}
    void error(const std::string&) override {}
    void warn(const std::string&) override {}
};

} // namespace device_reminder

TEST(BuzzerProcessTest, RunStartsAndStopsWatchDog) {
    using namespace device_reminder;

    auto wd = std::make_shared<StrictMock<MockWatchDog>>();
    // Set stop flag so ProcessBase::run exits immediately
    BuzzerProcess flag_setter(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    flag_setter.stop();

    BuzzerProcess proc(nullptr, nullptr, nullptr, nullptr, nullptr, wd);

    EXPECT_CALL(*wd, start()).Times(1);
    EXPECT_CALL(*wd, stop()).Times(1);

    EXPECT_EQ(proc.run(), 0);
}

TEST(BuzzerProcessTest, StopCallsWatchDogStop) {
    using namespace device_reminder;

    auto wd = std::make_shared<StrictMock<MockWatchDog>>();
    BuzzerProcess proc(nullptr, nullptr, nullptr, nullptr, nullptr, wd);

    EXPECT_CALL(*wd, stop()).Times(1);
    proc.stop();
}

TEST(BuzzerProcessTest, RunWithoutWatchDogWorks) {
    using namespace device_reminder;

    BuzzerProcess flag_setter(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    flag_setter.stop();

    BuzzerProcess proc(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    EXPECT_EQ(proc.run(), 0);
}

TEST(BuzzerProcessTest, StopWithoutWatchDogDoesNothing) {
    using namespace device_reminder;

    BuzzerProcess proc(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    EXPECT_NO_THROW(proc.stop());
}

