#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/watch_dog/watch_dog.hpp"

using ::testing::StrictMock;

namespace device_reminder {

class MockTimer : public ITimerService {
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};

TEST(WatchDogTest, ConstructWithValidTimer) {
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    WatchDog wd(timer);

    EXPECT_CALL(*timer, start()).Times(1);
    wd.start();
}

TEST(WatchDogTest, ConstructWithNullptr) {
    WatchDog wd(nullptr);
    EXPECT_NO_THROW(wd.start());
}

TEST(WatchDogTest, StartStartsTimerOnce) {
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    WatchDog wd(timer);

    EXPECT_CALL(*timer, start()).Times(1);
    wd.start();
}

TEST(WatchDogTest, StartWhenRunningDoesNothing) {
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    WatchDog wd(timer);

    EXPECT_CALL(*timer, start()).Times(1);
    wd.start();
    wd.start();
}

TEST(WatchDogTest, StartWithNullTimerDoesNothing) {
    WatchDog wd(nullptr);
    EXPECT_NO_THROW(wd.start());
}

TEST(WatchDogTest, StopStopsTimer) {
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    WatchDog wd(timer);

    EXPECT_CALL(*timer, start()).Times(1);
    EXPECT_CALL(*timer, stop()).Times(1);
    wd.start();
    wd.stop();
}

TEST(WatchDogTest, StopWithNullTimerDoesNothing) {
    WatchDog wd(nullptr);
    EXPECT_NO_THROW(wd.stop());
}

TEST(WatchDogTest, StopWhenNotRunningDoesNothing) {
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    WatchDog wd(timer);

    EXPECT_CALL(*timer, stop()).Times(0);
    wd.stop();
}

TEST(WatchDogTest, KickRestartsTimer) {
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    WatchDog wd(timer);

    {
        ::testing::InSequence s;
        EXPECT_CALL(*timer, start()).Times(1); // from start
        EXPECT_CALL(*timer, stop()).Times(1);  // from kick
        EXPECT_CALL(*timer, start()).Times(1); // from kick
    }

    wd.start();
    wd.kick();
}

TEST(WatchDogTest, KickWithNullTimerDoesNothing) {
    WatchDog wd(nullptr);
    EXPECT_NO_THROW(wd.kick());
}

} // namespace device_reminder
