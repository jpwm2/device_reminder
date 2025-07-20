#include <gtest/gtest.h>

#include "infra/message_codec/codec.hpp"

using namespace device_reminder;

TEST(MessageCodecTest, EncodeDecodeRoundtrip) {
    Message msg{MessageType::BuzzerOn, true};
    auto raw = encode(msg);
    auto decoded = decode(raw.data(), raw.size());
    ASSERT_TRUE(decoded);
    EXPECT_EQ(decoded->type(), msg.type());
    EXPECT_EQ(decoded->payload(), msg.payload());
}
