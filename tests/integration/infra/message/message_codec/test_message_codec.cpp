#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/message/message_codec/message_codec.hpp"
#include "infra/message/message.hpp"
#include "infra/logger/logger.hpp"

#include <spdlog/sinks/sink.h>
#include <spdlog/logger.h>

namespace {
class MockSink : public spdlog::sinks::sink {
public:
    MOCK_METHOD(void, log, (const spdlog::details::log_msg&), (override));
    MOCK_METHOD(void, flush, (), (override));
    MOCK_METHOD(void, set_pattern, (const std::string&), (override));
    MOCK_METHOD(void, set_formatter, (std::unique_ptr<spdlog::formatter>), (override));
};
} // namespace

namespace device_reminder {
namespace {
using ::testing::StrictMock;
}

TEST(MessageCodecIntegrationTest, EncodeDecodeSuccess) {
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto spd_logger = std::make_shared<spdlog::logger>("test_logger", sink);
    spd_logger->set_level(spdlog::level::trace);
    auto logger = std::make_shared<Logger>(spd_logger);
    MessageCodec codec(logger);

    auto msg = std::make_shared<Message>(
        MessageType::HumanFound,
        std::vector<std::string>{"hello", "world"},
        logger);

    EXPECT_CALL(*sink, log).Times(0);
    EXPECT_CALL(*sink, flush).Times(0);

    auto encoded = codec.encode(msg);
    auto decoded = codec.decode(encoded);

    ASSERT_NE(decoded, nullptr);
    EXPECT_EQ(decoded->type(), msg->type());
    EXPECT_EQ(decoded->payload(), msg->payload());
}

TEST(MessageCodecIntegrationTest, DecodeFailureLogsError) {
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto spd_logger = std::make_shared<spdlog::logger>("test_logger", sink);
    spd_logger->set_level(spdlog::level::trace);
    auto logger = std::make_shared<Logger>(spd_logger);
    MessageCodec codec(logger);

    EXPECT_CALL(*sink, flush).Times(0);
    EXPECT_CALL(*sink, log(::testing::Truly([](const spdlog::details::log_msg& msg) {
        std::string payload(msg.payload.data(), msg.payload.size());
        return payload.find("data too short") != std::string::npos;
    }))).Times(1);

    EXPECT_THROW(codec.decode({}), std::invalid_argument);
}

} // namespace device_reminder
