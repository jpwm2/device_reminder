#include <gtest/gtest.h>
#include "gpio_driver/gpio_driver.hpp"
#include "stubs/gpiod_stub.h"

using namespace device_reminder;

class GPIODriverTest : public ::testing::Test {
protected:
    void SetUp() override { gpiod_stub_reset(); }
    void TearDown() override {}
};

TEST_F(GPIODriverTest, OpenChipThrowsOnFailure) {
    gpiod_stub_set_fail_chip_open(1);
    GPIODriver driver{nullptr};
    EXPECT_THROW(driver.openChip("chip0"), std::runtime_error);
}

TEST_F(GPIODriverTest, SetupLineRequiresOpenChip) {
    GPIODriver driver{nullptr};
    EXPECT_THROW(driver.setupLine(1), std::runtime_error);
}

TEST_F(GPIODriverTest, SetupLineThrowsWhenRequestFails) {
    GPIODriver driver{nullptr};
    driver.openChip("chip0");
    gpiod_stub_set_request_input_result(-1);
    EXPECT_THROW(driver.setupLine(1), std::runtime_error);
}

TEST_F(GPIODriverTest, ReadLineReturnsValue) {
    GPIODriver driver{nullptr};
    driver.openChip("chip0");
    driver.setupLine(1);
    gpiod_stub_set_get_value_result(1);
    EXPECT_EQ(driver.readLine(), 1);
}

TEST_F(GPIODriverTest, ReadLineThrowsOnError) {
    GPIODriver driver{nullptr};
    driver.openChip("chip0");
    driver.setupLine(1);
    gpiod_stub_set_get_value_result(-1);
    EXPECT_THROW(driver.readLine(), std::runtime_error);
}
