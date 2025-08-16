#include <gtest/gtest.h>

#include "infra/gpio_operation/gpio_setter.hpp"
#include "stubs/gpiod_stub.h"

using namespace device_reminder;

TEST(GPIOSetterTest, WriteSetsHigh) {
    gpiod_stub_reset();
    gpiod_stub_set_set_value_result(0);
    GPIOSetter setter(nullptr, 1, nullptr);
    setter.write(true);
    EXPECT_EQ(gpiod_stub_get_last_set_value(), 1);
}
