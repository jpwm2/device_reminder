#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/logger/logger.hpp"

#include <spdlog/logger.h>
#include <spdlog/sinks/sink.h>

using ::testing::StrictMock;

class MockSink : public spdlog::sinks::sink {
public:
    MOCK_METHOD(void, log, (const spdlog::details::log_msg& msg), (override));
    MOCK_METHOD(void, flush, (), (override));
    MOCK_METHOD(void, set_pattern, (const std::string& pattern), (override));
    void set_formatter(std::unique_ptr<spdlog::formatter>) override {}
};

TEST(LoggerTest, InfoOutputsToSink) {
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    device_reminder::Logger logger(spdlogger);

    const std::string message = "info message";

    EXPECT_CALL(*sink, log(testing::Truly([&](const spdlog::details::log_msg& msg) {
        return msg.level == spdlog::level::info && msg.payload == message;
    })));

    logger.info(message);
}

TEST(LoggerTest, ErrorOutputsToSink) {
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    device_reminder::Logger logger(spdlogger);

    const std::string message = "error message";

    EXPECT_CALL(*sink, log(testing::Truly([&](const spdlog::details::log_msg& msg) {
        return msg.level == spdlog::level::err && msg.payload == message;
    })));

    logger.error(message);
}

