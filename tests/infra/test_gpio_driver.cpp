#include <gtest/gtest.h>
#include "gpio_driver/gpio_driver.hpp"
#include "stubs/gpiod_stub.h"

using namespace device_reminder;

class GPIODriverTest : public ::testing::Test {
protected:
    void SetUp() override { gpiod_stub_reset(); }
};

TEST_F(GPIODriverTest, ConstructorThrowsWhenChipOpenFails) {
    gpiod_stub_set_fail_chip_open(1);
    EXPECT_THROW({ GPIODriver driver(1, Direction::INPUT, nullptr); }, std::runtime_error);
}

TEST_F(GPIODriverTest, WriteCallsSetValue) {
    GPIODriver driver(1, Direction::OUTPUT, nullptr);
    gpiod_stub_set_set_value_result(0);
    EXPECT_NO_THROW(driver.write(true));
}

TEST_F(GPIODriverTest, ReadReturnsValue) {
    GPIODriver driver(1, Direction::INPUT, nullptr);
    gpiod_stub_set_get_value_result(1);
    EXPECT_TRUE(driver.read());
}

TEST_F(GPIODriverTest, WaitForEdgeUsesEventWait) {
    GPIODriver driver(1, Direction::INPUT, nullptr);
    gpiod_stub_set_event_wait_result(1);
    EXPECT_NO_THROW(driver.waitForEdge());
}

