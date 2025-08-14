#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/message/message_codec.hpp"
#include "infra/message/message.hpp"
#include "infra/logger.hpp"

using ::testing::StrictMock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Throw;

namespace device_reminder {

class MockProcessMessage : public IProcessMessage {
public:
    MOCK_METHOD(ProcessMessageType, type, (), (const, noexcept, override));
    MOCK_METHOD(std::vector<std::string>, payload, (), (const, noexcept, override));
    MOCK_METHOD(std::shared_ptr<IProcessMessage>, clone, (), (const, override));
    MOCK_METHOD(std::string, to_string, (), (const, override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

static std::vector<uint8_t> make_bytes(std::initializer_list<uint8_t> list) {
    return std::vector<uint8_t>(list.begin(), list.end());
}

TEST(MessageCodecTest, EncodeNullReturnsEmpty) {
    MessageCodec codec(nullptr);
    auto out = codec.encode(nullptr);
    EXPECT_TRUE(out.empty());
}

TEST(MessageCodecTest, EncodeSimpleMessage) {
    MessageCodec codec(nullptr);
    auto msg = std::make_shared<ProcessMessage>(
        ProcessMessageType::StartBuzzing,
        std::vector<std::string>{"1"},
        nullptr);
    auto out = codec.encode(msg);
    std::vector<uint8_t> expected = make_bytes({
        static_cast<uint8_t>(ProcessMessageType::StartBuzzing),
        1,0,0,0,
        1,0,0,0,
        '1'});
    EXPECT_EQ(out, expected);
}

TEST(MessageCodecTest, EncodeCallsMessageMethods) {
    MessageCodec codec(nullptr);
    StrictMock<MockProcessMessage> mock;
    EXPECT_CALL(mock, type()).WillOnce(Return(ProcessMessageType::StartBuzzing));
    EXPECT_CALL(mock, payload()).WillOnce(Return(std::vector<std::string>{"x"}));
    auto out = codec.encode(std::shared_ptr<IProcessMessage>(&mock, [](IProcessMessage*){}));
    std::vector<uint8_t> expected = make_bytes({5,1,0,0,0,1,0,0,0,'x'});
    EXPECT_EQ(out, expected);
}

TEST(MessageCodecTest, DecodeValidMessage) {
    MessageCodec codec(nullptr);
    std::vector<uint8_t> data = make_bytes({5,1,0,0,0,1,0,0,0,'1'});
    auto res = codec.decode(data);
    ASSERT_NE(res, nullptr);
    EXPECT_EQ(res->type(), ProcessMessageType::StartBuzzing);
    EXPECT_EQ(res->payload(), std::vector<std::string>{"1"});
}

TEST(MessageCodecTest, DecodeEmptyDataLogsError) {
    StrictMock<MockLogger> logger;
    MessageCodec codec(std::shared_ptr<ILogger>(&logger, [](ILogger*){}));
    EXPECT_CALL(logger, error(testing::HasSubstr("data too short"))).Times(1);
    auto res = codec.decode({});
    EXPECT_EQ(res, nullptr);
}

TEST(MessageCodecTest, DecodeOverflowLogsError) {
    StrictMock<MockLogger> logger;
    MessageCodec codec(std::shared_ptr<ILogger>(&logger, [](ILogger*){}));
    EXPECT_CALL(logger, error(std::string("decode overflow"))).Times(1);
    auto res = codec.decode(make_bytes({5}));
    EXPECT_EQ(res, nullptr);
}

TEST(MessageCodecTest, DecodeTooShortPayloadLogsError) {
    StrictMock<MockLogger> logger;
    MessageCodec codec(std::shared_ptr<ILogger>(&logger, [](ILogger*){}));
    EXPECT_CALL(logger, error(testing::HasSubstr("data too short"))).Times(1);
    auto res = codec.decode(make_bytes({5,1,0,0,0,3,0,0,0,'A','B'}));
    EXPECT_EQ(res, nullptr);
}

TEST(MessageCodecTest, DecodeWithoutLoggerReturnsNull) {
    MessageCodec codec(nullptr);
    auto res = codec.decode({});
    EXPECT_EQ(res, nullptr);
}

} // namespace device_reminder

