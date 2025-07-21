#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/buzzer_driver/buzzer_driver.hpp"
#include "infra/gpio_driver/i_gpio_driver.hpp"
#include <filesystem>
#include <fstream>

using namespace device_reminder;
using ::testing::NiceMock;

namespace {
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
};
class MockGPIO : public IGPIODriver {
public:
    MOCK_METHOD(void, write, (bool), (override));
    MOCK_METHOD(bool, read, (), (override));
    MOCK_METHOD(void, setEdge, (EdgeType), (override));
    MOCK_METHOD(void, waitForEdge, (), (override));
};
} // namespace

TEST(BuzzerDriverTest, StartStopWritesSysfs) {
    namespace fs = std::filesystem;
    fs::path base = fs::temp_directory_path() / "pwm_test";
    fs::remove_all(base);
    fs::create_directories(base / "pwmchip0" / "pwm0");
    std::ofstream(base / "pwmchip0" / "export");
    std::ofstream(base / "pwmchip0" / "unexport");
    std::ofstream(base / "pwmchip0" / "pwm0" / "period");
    std::ofstream(base / "pwmchip0" / "pwm0" / "duty_cycle");
    std::ofstream(base / "pwmchip0" / "pwm0" / "enable");

    NiceMock<MockGPIO> gpio;
    NiceMock<MockLogger> logger;
    BuzzerDriver driver(&gpio, &logger, "/dev/gpiochip0", 18, 261.63, 0.5, base.string());

    std::string val;
    std::ifstream(base / "pwmchip0" / "pwm0" / "enable") >> val;
    EXPECT_EQ(val, "0");

    EXPECT_TRUE(driver.start());
    val.clear();
    std::ifstream(base / "pwmchip0" / "pwm0" / "enable") >> val;
    EXPECT_EQ(val, "1");

    EXPECT_TRUE(driver.stop());
    val.clear();
    std::ifstream(base / "pwmchip0" / "pwm0" / "enable") >> val;
    EXPECT_EQ(val, "0");

    fs::remove_all(base);
}
