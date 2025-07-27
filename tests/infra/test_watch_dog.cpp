#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "watch_dog/watch_dog.hpp"

using ::testing::NiceMock;
using ::testing::StrictMock;

namespace device_reminder {

class MockTimer : public ITimerService {
public:
    MOCK_METHOD(void, start, (uint32_t, const ProcessMessage&), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, active, (), (const, noexcept, override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

TEST(WatchDogTest, KickRestartsTimer) {
    auto timer = std::make_shared<StrictMock<MockTimer>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    EXPECT_CALL(*timer, start(100, testing::_)).Times(2);

    WatchDog wd(logger, timer, []{}, 100);
    wd.start();
    wd.kick();
}

TEST(WatchDogTest, TimeoutInvokesHandler) {
    class FakeTimer : public ITimerService {
    public:
        std::function<void()> handler;
        void start(uint32_t, const ProcessMessage&) override {
            if (handler) handler();
        }
        void stop() override {}
        bool active() const noexcept override { return false; }
    };

    auto timer = std::make_shared<FakeTimer>();
    bool called = false;
    timer->handler = [&]{ called = true; };
    NiceMock<MockLogger> logger;

    WatchDog wd(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), timer,
                [&]{ called = true; }, 10);
    wd.start();
    EXPECT_TRUE(called);
}

} // namespace device_reminder

