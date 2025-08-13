#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/logger.hpp"

#include <spdlog/logger.h>
#include <spdlog/sinks/sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <fstream>

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

TEST(LoggerTest, WarnOutputsToSink) {
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    device_reminder::Logger logger(spdlogger);

    const std::string message = "warn message";

    EXPECT_CALL(*sink, log(testing::Truly([&](const spdlog::details::log_msg& msg) {
        return msg.level == spdlog::level::warn && msg.payload == message;
    })));

    logger.warn(message);
}

TEST(LoggerTest, FileSinkWritesLog) {
    const std::string filename = "log.txt";
    auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true);
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    device_reminder::Logger logger(spdlogger);

    const std::string message = "file output message";
    logger.info(message);
    spdlogger->flush();

    std::ifstream ifs(filename);
    ASSERT_TRUE(ifs.is_open());
    std::string line;
    bool found = false;
    while (std::getline(ifs, line)) {
        if (line.find(message) != std::string::npos) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

class ThrowSink : public spdlog::sinks::sink {
public:
    void log(const spdlog::details::log_msg&) override { throw std::runtime_error("boom"); }
    void flush() override {}
    void set_pattern(const std::string&) override {}
    void set_formatter(std::unique_ptr<spdlog::formatter>) override {}
};

TEST(LoggerTest, PropagatesExceptionsFromSink) {
    auto sink = std::make_shared<ThrowSink>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    device_reminder::Logger logger(spdlogger);

    EXPECT_NO_THROW(logger.info("boom"));
}

