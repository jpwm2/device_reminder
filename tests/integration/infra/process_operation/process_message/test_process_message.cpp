#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cstdint>
#include <vector>

#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/process_operation/message_codec/message_codec.hpp"
#include "infra/logger/i_logger.hpp"

namespace device_reminder {

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string& msg), (override));
    MOCK_METHOD(void, error, (const std::string& msg), (override));
    MOCK_METHOD(void, warn, (const std::string& msg), (override));
};

// 正常系: メッセージをエンコードしてデコードした結果が元と一致し、エラーが出力されない
TEST(ProcessMessageIntegrationTest, EncodeAndDecodeSuccess) {
    auto logger = std::make_shared<MockLogger>();
    EXPECT_CALL(*logger, error(::testing::_)).Times(0);

    MessageCodec codec(logger);

    std::vector<std::string> payload{"hello", "world"};
    auto message = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, payload);

    auto encoded = codec.encode(message);
    auto decoded = codec.decode(encoded);

    ASSERT_NE(decoded, nullptr);
    EXPECT_EQ(decoded->type(), ProcessMessageType::StartBuzzing);
    EXPECT_EQ(decoded->payload(), payload);
}

// 異常系: サイズ不足のデータをデコードした場合、nullptrが返り、エラーがログ出力される
TEST(ProcessMessageIntegrationTest, DecodeFailureWhenDataTooShort) {
    auto logger = std::make_shared<MockLogger>();
    EXPECT_CALL(*logger, error(::testing::StrEq("data too short"))).Times(1);

    MessageCodec codec(logger);

    std::vector<uint8_t> invalid_data; // 空のデータ
    auto decoded = codec.decode(invalid_data);

    EXPECT_EQ(decoded, nullptr);
}

} // namespace device_reminder

