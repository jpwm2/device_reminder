
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/gpio_operation/gpio_setter/gpio_setter.hpp"
#include "infra/logger/logger.hpp"
#include "stubs/gpiod_stub.h"

#include <spdlog/logger.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/sink.h>
#include <memory>
#include <string>

using namespace device_reminder;

using ::testing::StrictMock;
using ::testing::InSequence;

class MockSink : public spdlog::sinks::sink {
public:
    MOCK_METHOD(void, log, (const spdlog::details::log_msg& msg), (override));
    MOCK_METHOD(void, flush, (), (override));
    MOCK_METHOD(void, set_pattern, (const std::string& pattern), (override));
    void set_formatter(std::unique_ptr<spdlog::formatter>) override {}
};

class GPIOSetterIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override { gpiod_stub_reset(); }
};

TEST_F(GPIOSetterIntegrationTest, WriteNormal) {
    auto sink = std::make_shared<spdlog::sinks::null_sink_mt>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    auto logger = std::make_shared<Logger>(spdlogger);

    GPIOSetter setter(logger, 1);
    gpiod_stub_set_set_value_result(0);

    EXPECT_NO_THROW(setter.write(true));
    EXPECT_EQ(1, gpiod_stub_get_last_set_value());

    EXPECT_NO_THROW(setter.write(false));
    EXPECT_EQ(0, gpiod_stub_get_last_set_value());
}

TEST_F(GPIOSetterIntegrationTest, WriteFailureLogsError) {
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    auto logger = std::make_shared<Logger>(spdlogger);

    {
        InSequence seq;
        EXPECT_CALL(*sink, log(testing::Truly([](const spdlog::details::log_msg& msg) {
                            return msg.level == spdlog::level::info &&
                                   std::string(msg.payload.data(), msg.payload.size()) ==
                                       "GPIOSetter initialized";
                        })));
        EXPECT_CALL(*sink, log(testing::Truly([](const spdlog::details::log_msg& msg) {
                            return msg.level == spdlog::level::err &&
                                   std::string(msg.payload.data(), msg.payload.size()) ==
                                       "Failed to write GPIO line value";
                        })));
        EXPECT_CALL(*sink, log(testing::Truly([](const spdlog::details::log_msg& msg) {
                            return msg.level == spdlog::level::info &&
                                   std::string(msg.payload.data(), msg.payload.size()) ==
                                       "GPIOSetter destroyed";
                        })));
    }

    GPIOSetter setter(logger, 1);
    gpiod_stub_set_set_value_result(-1);

    EXPECT_THROW(setter.write(true), std::runtime_error);
    EXPECT_EQ(1, gpiod_stub_get_last_set_value());
}
