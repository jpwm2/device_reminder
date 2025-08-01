#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/gpio_operation/gpio_reader/gpio_reader.hpp"
#include "stubs/gpiod_stub.h"

using namespace device_reminder;

class GPIOReaderTest : public ::testing::Test {
protected:
    void SetUp() override { gpiod_stub_reset(); }
};

TEST_F(GPIOReaderTest, ConstructorThrowsWhenChipOpenFails) {
    gpiod_stub_set_fail_chip_open(1);
    EXPECT_THROW({ GPIOReader reader(nullptr, 1); }, std::runtime_error);
}

TEST_F(GPIOReaderTest, ReadReturnsTrueForHigh) {
    gpiod_stub_set_get_value_result(1);
    GPIOReader reader(nullptr, 1);
    EXPECT_TRUE(reader.read());
}

TEST_F(GPIOReaderTest, ReadReturnsFalseForLow) {
    gpiod_stub_set_get_value_result(0);
    GPIOReader reader(nullptr, 1);
    EXPECT_FALSE(reader.read());
}

