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
    MOCK_METHOD(void, openChip, (const std::string&), (override));
    MOCK_METHOD(void, setupLine, (unsigned int), (override));
    MOCK_METHOD(int, readLine, (), (override));
    MOCK_METHOD(void, close, (), (override));
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
    EXPECT_CALL(gpio, openChip("/dev/gpiochip0"));
    EXPECT_CALL(gpio, setupLine(17));
    EXPECT_CALL(logger, info(testing::_)).Times(testing::AtLeast(1));
    EXPECT_CALL(gpio, close());
    {
        PIRDriver driver(&gpio, &logger, 17, "/dev/gpiochip0");
        EXPECT_CALL(gpio, readLine()).WillOnce(testing::Return(1));
        EXPECT_EQ(driver.read(), 1);
    }
}
