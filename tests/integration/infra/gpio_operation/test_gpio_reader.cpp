#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/gpio_operation/gpio_reader/gpio_reader.hpp"

#include "infra/logger.hpp"
#include "stubs/gpiod_stub.h"

using namespace device_reminder;
using ::testing::StrictMock;
using ::testing::NiceMock;

namespace {
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};
} // namespace


class GPIOReaderTest : public ::testing::Test {
protected:
    void SetUp() override { gpiod_stub_reset(); }
};


TEST_F(GPIOReaderTest, ConstructorSuccessDefault) {
    EXPECT_NO_THROW({ GPIOReader reader(nullptr, 1); });
}

TEST_F(GPIOReaderTest, ConstructorSuccessCustomChip) {
    EXPECT_NO_THROW({ GPIOReader reader(nullptr, 2, "/dev/customchip"); });
}


TEST_F(GPIOReaderTest, ConstructorThrowsWhenChipOpenFails) {
    gpiod_stub_set_fail_chip_open(1);
    EXPECT_THROW({ GPIOReader reader(nullptr, 1); }, std::runtime_error);
}


TEST_F(GPIOReaderTest, ConstructorThrowsWhenGetLineFails) {
    gpiod_stub_set_fail_get_line(1);
    EXPECT_THROW({ GPIOReader reader(nullptr, 1); }, std::runtime_error);
}

TEST_F(GPIOReaderTest, ConstructorThrowsWhenRequestInputFails) {
    gpiod_stub_set_request_input_result(-1);
    EXPECT_THROW({ GPIOReader reader(nullptr, 1); }, std::runtime_error);
}

TEST_F(GPIOReaderTest, ConstructorAcceptsLogger) {
    NiceMock<MockLogger> logger;
    EXPECT_NO_THROW({
        GPIOReader reader(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), 1);
    });
}

TEST_F(GPIOReaderTest, ConstructorLogsErrorWhenGetLineFails) {
    StrictMock<MockLogger> logger;
    gpiod_stub_set_fail_get_line(1);
    EXPECT_CALL(logger, error(testing::_)).Times(1);
    EXPECT_THROW({
        GPIOReader reader(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), 1);
    }, std::runtime_error);
}

TEST_F(GPIOReaderTest, ConstructorLogsErrorWhenRequestInputFails) {
    StrictMock<MockLogger> logger;
    gpiod_stub_set_request_input_result(-1);
    EXPECT_CALL(logger, error(testing::_)).Times(1);
    EXPECT_THROW({
        GPIOReader reader(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), 1);
    }, std::runtime_error);
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

TEST_F(GPIOReaderTest, ReadThrowsOnFailure) {
    gpiod_stub_set_get_value_result(-1);
    GPIOReader reader(nullptr, 1);
    EXPECT_THROW(reader.read(), std::runtime_error);
}

TEST_F(GPIOReaderTest, ReadLogsErrorOnFailure) {
    StrictMock<MockLogger> logger;
    gpiod_stub_set_get_value_result(-1);
    GPIOReader reader(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), 1);
    EXPECT_CALL(logger, error(testing::_)).Times(1);
    EXPECT_THROW(reader.read(), std::runtime_error);
}

TEST_F(GPIOReaderTest, ReadDoesNotLogOnSuccess) {
    StrictMock<MockLogger> logger;
    gpiod_stub_set_get_value_result(1);
    EXPECT_CALL(logger, error(testing::_)).Times(0);
    GPIOReader reader(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), 1);
    EXPECT_TRUE(reader.read());
}

