#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/buzzer_driver/buzzer_driver.hpp"
#include "infra/gpio_operation/gpio_setter/i_gpio_setter.hpp"

using namespace device_reminder;
using ::testing::NiceMock;

namespace {
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};
class MockGPIO : public IGPIOSetter {
public:
    MOCK_METHOD(void, write, (bool), (override));
};
} // namespace

TEST(BuzzerDriverTest, OnOffCallsGPIOWrite) {
    auto gpio = std::make_shared<NiceMock<MockGPIO>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    BuzzerDriver driver(nullptr, logger, gpio);

    EXPECT_CALL(*gpio, write(true)).Times(1);
    driver.on();

    EXPECT_CALL(*gpio, write(false)).Times(1);
    driver.off();
}
