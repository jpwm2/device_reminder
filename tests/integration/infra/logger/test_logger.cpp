#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/logger/logger.hpp"

#include <spdlog/logger.h>
#include <spdlog/sinks/sink.h>
#include <memory>

using ::testing::StrictMock;
using ::testing::Truly;

// 下位モジュールのモック（spdlog の sink）
class MockSink : public spdlog::sinks::sink {
public:
    MOCK_METHOD(void, log, (const spdlog::details::log_msg& msg), (override));
    MOCK_METHOD(void, flush, (), (override));
    MOCK_METHOD(void, set_pattern, (const std::string& pattern), (override));
    void set_formatter(std::unique_ptr<spdlog::formatter>) override {}
};

// 例外を送出するシンク
class ThrowSink : public spdlog::sinks::sink {
public:
    void log(const spdlog::details::log_msg&) override { throw std::runtime_error("boom"); }
    void flush() override {}
    void set_pattern(const std::string&) override {}
    void set_formatter(std::unique_ptr<spdlog::formatter>) override {}
};

using device_reminder::Logger;

// 正常系: info
TEST(LoggerIntegrationTest, InfoOutputsToSink) {
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    Logger logger(spdlogger);

    const std::string message = "info message";

    EXPECT_CALL(*sink, log(Truly([&](const spdlog::details::log_msg& msg) {
        return msg.level == spdlog::level::info && msg.payload == message;
    })));

    logger.info(message);
}

// 正常系: error
TEST(LoggerIntegrationTest, ErrorOutputsToSink) {
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    Logger logger(spdlogger);

    const std::string message = "error message";

    EXPECT_CALL(*sink, log(Truly([&](const spdlog::details::log_msg& msg) {
        return msg.level == spdlog::level::err && msg.payload == message;
    })));

    logger.error(message);
}

// 正常系: warn
TEST(LoggerIntegrationTest, WarnOutputsToSink) {
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    Logger logger(spdlogger);

    const std::string message = "warn message";

    EXPECT_CALL(*sink, log(Truly([&](const spdlog::details::log_msg& msg) {
        return msg.level == spdlog::level::warn && msg.payload == message;
    })));

    logger.warn(message);
}

// 異常系: シンクが例外を投げても例外が伝播しない
TEST(LoggerIntegrationTest, SinkThrowsButLoggerDoesNot) {
    auto sink = std::make_shared<ThrowSink>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    Logger logger(spdlogger);

    EXPECT_NO_THROW(logger.info("boom"));
}

