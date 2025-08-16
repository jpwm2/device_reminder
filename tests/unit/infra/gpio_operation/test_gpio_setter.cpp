#include <gtest/gtest.h>

#include <gmock/gmock.h>

#include "infra/gpio_operation/gpio_setter/gpio_setter.hpp"
#include "infra/logger.hpp"
#include "stubs/gpiod_stub.h"
#include <memory>

using namespace device_reminder;

using ::testing::NiceMock;
using ::testing::StrictMock;

namespace {
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};
} // namespace

class GPIOSetterTest : public ::testing::Test {
protected:
    void SetUp() override { gpiod_stub_reset(); }
};

// ---- Constructor Tests ----

TEST_F(GPIOSetterTest, ConstructWithNullLoggerSucceeds) {
    EXPECT_NO_THROW({ GPIOSetter setter(nullptr, 1); });
}

TEST_F(GPIOSetterTest, ConstructWithCustomChipNameSucceeds) {
    EXPECT_NO_THROW({ GPIOSetter setter(nullptr, 5, "testchip"); });
}

TEST_F(GPIOSetterTest, ConstructorThrowsWhenChipOpenFails) {
    gpiod_stub_set_fail_chip_open(1);
    EXPECT_THROW({ GPIOSetter setter(nullptr, 1); }, std::runtime_error);
}

TEST_F(GPIOSetterTest, ConstructorThrowsWhenGetLineFails) {
    gpiod_stub_set_fail_get_line(1);
    EXPECT_THROW({ GPIOSetter setter(nullptr, 1); }, std::runtime_error);
}

TEST_F(GPIOSetterTest, ConstructorThrowsWhenLineRequestFails) {
    gpiod_stub_set_request_output_result(-1);
    EXPECT_THROW({ GPIOSetter setter(nullptr, 1); }, std::runtime_error);
}

TEST_F(GPIOSetterTest, ConstructorWithLoggerLogsInfo) {
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    {
        ::testing::InSequence seq;
        EXPECT_CALL(*logger, info("GPIOSetter initialized"));
        EXPECT_CALL(*logger, info("GPIOSetter destroyed"));
        GPIOSetter setter(logger, 1);
    }
}

TEST_F(GPIOSetterTest, ConstructorThrowsWithNullLoggerWhenGetLineFails) {
    gpiod_stub_set_fail_get_line(1);
    EXPECT_THROW({ GPIOSetter setter(nullptr, 1); }, std::runtime_error);
}

TEST_F(GPIOSetterTest, ConstructorFailureLogsError) {
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    gpiod_stub_set_fail_chip_open(1);
    EXPECT_CALL(*logger, error(testing::StartsWith("Failed to open GPIO chip")));
    EXPECT_THROW({ GPIOSetter setter(logger, 1); }, std::runtime_error);
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

// ---- Write Method Tests ----

TEST_F(GPIOSetterTest, WriteFalseSuccess) {
    GPIOSetter setter(nullptr, 1);
    gpiod_stub_set_set_value_result(0);
    EXPECT_NO_THROW(setter.write(false));
}

TEST_F(GPIOSetterTest, WriteWithLoggerNoError) {
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    {
        ::testing::InSequence seq;
        EXPECT_CALL(*logger, info("GPIOSetter initialized"));

        EXPECT_CALL(*logger, error(testing::_)).Times(0);
        EXPECT_CALL(*logger, info("GPIOSetter destroyed"));
        GPIOSetter setter(logger, 1);
        gpiod_stub_set_set_value_result(0);
        EXPECT_NO_THROW(setter.write(true));
    }
}

TEST_F(GPIOSetterTest, WriteFailureWithNullLoggerThrows) {
    GPIOSetter setter(nullptr, 1);
    gpiod_stub_set_set_value_result(-1);
    EXPECT_THROW(setter.write(true), std::runtime_error);
}

TEST_F(GPIOSetterTest, WriteFailureLogsError) {
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    {
        ::testing::InSequence seq;
        EXPECT_CALL(*logger, info("GPIOSetter initialized"));
        EXPECT_CALL(*logger, error("Failed to write GPIO line value"));
        EXPECT_CALL(*logger, info("GPIOSetter destroyed"));
        GPIOSetter setter(logger, 1);
        gpiod_stub_set_set_value_result(-1);
        EXPECT_THROW(setter.write(true), std::runtime_error);
    }
}

// ---- Buzz Stop Tests ----

TEST_F(GPIOSetterTest, BuzzStopSuccess) {
    GPIOSetter setter(nullptr, 1);
    gpiod_stub_set_set_value_result(0);
    gpiod_stub_set_get_value_result(0);
    EXPECT_NO_THROW(setter.buzz_stop());
}

TEST_F(GPIOSetterTest, BuzzStopThrowsWhenStateHigh) {
    GPIOSetter setter(nullptr, 1);
    gpiod_stub_set_set_value_result(0);
    gpiod_stub_set_get_value_result(1);
    EXPECT_THROW(setter.buzz_stop(), std::runtime_error);
}

TEST_F(GPIOSetterTest, BuzzStopThrowsWhenGetValueFails) {
    GPIOSetter setter(nullptr, 1);
    gpiod_stub_set_set_value_result(0);
    gpiod_stub_set_get_value_result(-1);
    EXPECT_THROW(setter.buzz_stop(), std::runtime_error);
}

// ---- Destructor Tests ----

TEST_F(GPIOSetterTest, DestructorLogsInfo) {
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    {
        ::testing::InSequence seq;
        EXPECT_CALL(*logger, info("GPIOSetter initialized"));
        EXPECT_CALL(*logger, info("GPIOSetter destroyed"));
        GPIOSetter setter(logger, 1);
    }
}

TEST_F(GPIOSetterTest, DestructorWithNullLoggerNoThrow) {
    {
        GPIOSetter setter(nullptr, 1);
    }
}
