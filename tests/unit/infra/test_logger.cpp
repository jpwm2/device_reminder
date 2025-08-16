#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/logger.hpp"

#include <spdlog/logger.h>
#include <spdlog/sinks/sink.h>

using namespace device_reminder;
using ::testing::StrictMock;

class MockSink : public spdlog::sinks::sink {
public:
    MOCK_METHOD(void, log, (const spdlog::details::log_msg& msg), (override));
    MOCK_METHOD(void, flush, (), (override));
    MOCK_METHOD(void, set_pattern, (const std::string& pattern), (override));
    void set_formatter(std::unique_ptr<spdlog::formatter>) override {}
};

TEST(LoggerTest, InfoIsForwarded) {
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    Logger logger(spdlogger);
    EXPECT_CALL(*sink, log(testing::Field(&spdlog::details::log_msg::level, spdlog::level::info)));
    logger.info("hello");
}

TEST(LoggerTest, ErrorIsForwarded) {
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
    Logger logger(spdlogger);
    EXPECT_CALL(*sink, log(testing::Field(&spdlog::details::log_msg::level, spdlog::level::err)));
    logger.error("bad");
}
