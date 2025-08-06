#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/message/message_codec/message_codec.hpp"
#include "infra/message/message.hpp"
#include "infra/logger/i_logger.hpp"

using ::testing::StrictMock;
using ::testing::NiceMock;
using ::testing::Return;

namespace device_reminder {

class MockMessage : public IMessage {
public:
    MOCK_METHOD(MessageType, type, (), (const, noexcept, override));
    MOCK_METHOD(std::vector<std::string>, payload, (), (const, noexcept, override));
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

TEST(MessageCodecTest, EncodeNullThrows) {
    StrictMock<MockLogger> logger;
    MessageCodec codec(std::shared_ptr<ILogger>(&logger, [](ILogger*){}));
    EXPECT_CALL(logger, error(::testing::_)).Times(1);
    EXPECT_THROW(codec.encode(nullptr), std::invalid_argument);
}

TEST(MessageCodecTest, EncodeSimpleMessage) {
    MessageCodec codec(nullptr);
    auto msg = std::make_shared<Message>(MessageType::StartBuzzer, std::vector<std::string>{"1"}, nullptr);
    auto out = codec.encode(msg);
    std::vector<uint8_t> expected = make_bytes({
        static_cast<uint8_t>(MessageType::StartBuzzer),
        1,0,0,0,
        '1'});
    EXPECT_EQ(out, expected);
}

TEST(MessageCodecTest, EncodeCallsMessageMethods) {
    MessageCodec codec(nullptr);
    StrictMock<MockMessage> mock;
    EXPECT_CALL(mock, type()).WillOnce(Return(MessageType::StartBuzzer));
    EXPECT_CALL(mock, payload()).WillOnce(Return(std::vector<std::string>{"x"}));
    auto out = codec.encode(std::shared_ptr<IMessage>(&mock, [](IMessage*){}));
    std::vector<uint8_t> expected = make_bytes({
        static_cast<uint8_t>(MessageType::StartBuzzer),
        1,0,0,0,'x'});
    EXPECT_EQ(out, expected);
}

TEST(MessageCodecTest, DecodeValidMessage) {
    MessageCodec codec(nullptr);
    std::vector<uint8_t> data = make_bytes({
        static_cast<uint8_t>(MessageType::StartBuzzer),
        1,0,0,0,'1'});
    auto res = codec.decode(data);
    ASSERT_NE(res, nullptr);
    EXPECT_EQ(res->type(), MessageType::StartBuzzer);
    EXPECT_EQ(res->payload(), std::vector<std::string>{"1"});
}

TEST(MessageCodecTest, DecodeEmptyDataThrows) {
    StrictMock<MockLogger> logger;
    MessageCodec codec(std::shared_ptr<ILogger>(&logger, [](ILogger*){}));
    EXPECT_CALL(logger, error(::testing::_)).Times(1);
    EXPECT_THROW(codec.decode({}), std::invalid_argument);
}

TEST(MessageCodecTest, DecodeTooShortPayloadThrows) {
    StrictMock<MockLogger> logger;
    MessageCodec codec(std::shared_ptr<ILogger>(&logger, [](ILogger*){}));
    EXPECT_CALL(logger, error(::testing::_)).Times(1);
    auto data = make_bytes({
        static_cast<uint8_t>(MessageType::StartBuzzer),
        3,0,0,0,'A','B'});
    EXPECT_THROW(codec.decode(data), std::runtime_error);
}

} // namespace device_reminder
