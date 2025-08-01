#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace device_reminder {
class IProcessQueue; class IProcessReceiver; class IWorkerDispatcher;
class IProcessSender; class IFileLoader; class ILogger; class IWatchDog;
}

using namespace device_reminder;

#include "main_task/main_process.hpp"


using ::testing::StrictMock;
using ::testing::NiceMock;


class MockWatchDog : public IWatchDog {
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(void, kick, (), (override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string& msg), (override));
    MOCK_METHOD(void, error, (const std::string& msg), (override));
    MOCK_METHOD(void, warn, (const std::string& msg), (override));
};

TEST(MainProcessTest, ConstructorLogsCreation) {
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    auto watchdog = std::make_shared<StrictMock<MockWatchDog>>();

    EXPECT_CALL(*logger, info("MainProcess created")).Times(1);

    MainProcess proc(nullptr, nullptr, nullptr, nullptr, logger, watchdog);
}

TEST(MainProcessTest, ConstructorWithNullLoggerDoesNotThrow) {
    auto watchdog = std::make_shared<StrictMock<MockWatchDog>>();

    EXPECT_NO_THROW(MainProcess(nullptr, nullptr, nullptr, nullptr, nullptr, watchdog));
}

TEST(MainProcessTest, RunStartsAndStopsWatchDog) {
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    auto watchdog = std::make_shared<StrictMock<MockWatchDog>>();

    MainProcess proc(nullptr, nullptr, nullptr, nullptr, logger, watchdog);

    EXPECT_CALL(*watchdog, start()).Times(1);
    EXPECT_CALL(*watchdog, stop()).Times(1);

    proc.stop(); // すぐに終了させる
    proc.run();
}

TEST(MainProcessTest, RunWithoutWatchDogDoesNothing) {
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    MainProcess proc(nullptr, nullptr, nullptr, nullptr, logger, nullptr);

    proc.stop();
    EXPECT_NO_THROW(proc.run());
}

TEST(MainProcessTest, StopCallsWatchDogAndProcessBase) {
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    auto watchdog = std::make_shared<StrictMock<MockWatchDog>>();

    MainProcess proc(nullptr, nullptr, nullptr, nullptr, logger, watchdog);

    EXPECT_CALL(*watchdog, stop()).Times(1);
    EXPECT_CALL(*logger, info("ProcessBase stop requested")).Times(1);

    proc.stop();
}

TEST(MainProcessTest, StopWithoutWatchDogCallsBaseOnly) {
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    MainProcess proc(nullptr, nullptr, nullptr, nullptr, logger, nullptr);

    EXPECT_CALL(*logger, info("ProcessBase stop requested")).Times(1);

    proc.stop();
}

