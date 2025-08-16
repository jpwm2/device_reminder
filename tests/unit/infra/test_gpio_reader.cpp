#include <gtest/gtest.h>

#include "infra/gpio_operation/gpio_reader.hpp"
#include "stubs/gpiod_stub.h"

using namespace device_reminder;

TEST(GPIOReaderTest, ReadHighReturnsTrue) {
    gpiod_stub_reset();
    gpiod_stub_set_get_value_result(1);
    GPIOReader reader(nullptr, 1, nullptr);
    EXPECT_TRUE(reader.read());
}

TEST(GPIOReaderTest, ReadLowReturnsFalse) {
    gpiod_stub_reset();
    gpiod_stub_set_get_value_result(0);
    GPIOReader reader(nullptr, 1, nullptr);
    EXPECT_FALSE(reader.read());
}
