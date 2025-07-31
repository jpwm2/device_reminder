#include <gtest/gtest.h>
#include "infra/gpio_operation/gpio_setter/gpio_setter.hpp"
#include "stubs/gpiod_stub.h"
#include <memory>

using namespace device_reminder;

class GPIOSetterTest : public ::testing::Test {
protected:
    void SetUp() override { gpiod_stub_reset(); }
};

TEST_F(GPIOSetterTest, ConstructorThrowsWhenChipOpenFails) {
    gpiod_stub_set_fail_chip_open(1);
    EXPECT_THROW({ GPIOSetter setter(nullptr, 1); }, std::runtime_error);
}

TEST_F(GPIOSetterTest, WriteSuccess) {
    GPIOSetter setter(nullptr, 1);
    gpiod_stub_set_set_value_result(0);
    EXPECT_NO_THROW(setter.write(true));
}

TEST_F(GPIOSetterTest, WriteThrowsOnFailure) {
    GPIOSetter setter(nullptr, 1);
    gpiod_stub_set_set_value_result(-1);
    EXPECT_THROW(setter.write(false), std::runtime_error);
}

