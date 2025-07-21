#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/pir_driver/pir_driver.hpp"
#include "infra/gpio_driver/i_gpio_driver.hpp"
#include "infra/logger/i_logger.hpp"

using namespace device_reminder;
using ::testing::StrictMock;

namespace {
class MockGPIO : public IGPIODriver {
public:
    MOCK_METHOD(void, write, (bool), (override));
    MOCK_METHOD(bool, read, (), (override));
    MOCK_METHOD(void, setEdge, (EdgeType), (override));
    MOCK_METHOD(void, waitForEdge, (), (override));
};
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
};
} // namespace

TEST(PIRDriverTest, InitCallsGPIO) {
    StrictMock<MockGPIO> gpio;
    StrictMock<MockLogger> logger;
    EXPECT_CALL(logger, info(testing::_)).Times(testing::AtLeast(1));
    {
        PIRDriver driver(&gpio, &logger);
        EXPECT_CALL(gpio, read()).WillOnce(testing::Return(true));
        EXPECT_EQ(driver.read(), 1);
    }
}
