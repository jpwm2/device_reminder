#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "infra/buzzer_driver/buzzer_driver.hpp"

using namespace device_reminder;
using ::testing::Return;
using ::testing::StrictMock;

namespace {

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
};

class MockGPIO : public IGPIOSetter {
public:
    MOCK_METHOD(void, write, (bool), (override));
};

class MockLoader : public IFileLoader {
public:
    MOCK_METHOD(int, load_int, (), (const, override));
    MOCK_METHOD(std::string, load_string, (), (const, override));
    MOCK_METHOD(std::vector<std::string>, load_string_list, (), (const, override));
};

} // namespace

TEST(BuzzerDriverTest, ConstructorInitializesGPIO) {
    auto loader = std::make_shared<StrictMock<MockLoader>>();
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    auto gpio   = std::make_shared<StrictMock<MockGPIO>>();

    {
        ::testing::InSequence seq;
        EXPECT_CALL(*loader, load_int()).WillOnce(Return(1));
        EXPECT_CALL(*loader, load_string()).WillOnce(Return("high"));
        EXPECT_CALL(*loader, load_string()).WillOnce(Return("off"));
        EXPECT_CALL(*gpio, write(false));
        EXPECT_CALL(*logger, info("BuzzerDriver init: pin=1 active_level=high initial_state=off"));
    }

    BuzzerDriver driver(loader, logger, gpio);
}

TEST(BuzzerDriverTest, OnOffOperations) {
    auto loader = std::make_shared<StrictMock<MockLoader>>();
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    auto gpio   = std::make_shared<StrictMock<MockGPIO>>();

    {
        ::testing::InSequence seq;
        EXPECT_CALL(*loader, load_int()).WillOnce(Return(2));
        EXPECT_CALL(*loader, load_string()).WillOnce(Return("low"));
        EXPECT_CALL(*loader, load_string()).WillOnce(Return("off"));
        EXPECT_CALL(*gpio, write(true));
        EXPECT_CALL(*logger, info("BuzzerDriver init: pin=2 active_level=low initial_state=off"));
    }

    BuzzerDriver driver(loader, logger, gpio);

    EXPECT_CALL(*gpio, write(false));
    EXPECT_CALL(*logger, info("Buzzer ON pin=2"));
    driver.on();

    EXPECT_CALL(*logger, info("Buzzer already ON"));
    driver.on();

    EXPECT_CALL(*gpio, write(true));
    EXPECT_CALL(*logger, info("Buzzer OFF pin=2"));
    driver.off();

    EXPECT_CALL(*logger, info("Buzzer already OFF"));
    driver.off();
}

TEST(BuzzerDriverTest, NullLoaderThrows) {
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    auto gpio   = std::make_shared<StrictMock<MockGPIO>>();

    EXPECT_CALL(*logger, error("IFileLoader is null"));
    EXPECT_THROW(BuzzerDriver(nullptr, logger, gpio), std::invalid_argument);
}

TEST(BuzzerDriverTest, NullLoggerThrows) {
    auto loader = std::make_shared<StrictMock<MockLoader>>();
    auto gpio   = std::make_shared<StrictMock<MockGPIO>>();

    EXPECT_THROW(BuzzerDriver(loader, nullptr, gpio), std::invalid_argument);
}

