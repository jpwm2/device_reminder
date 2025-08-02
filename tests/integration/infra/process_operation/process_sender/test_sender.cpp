#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/process_operation/process_sender/process_sender.hpp"
#include "infra/process_operation/process_queue/process_queue.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "tests/stubs/posix_mq_stub.h"

#include <unistd.h>
#include <ctime>

using namespace device_reminder;
using ::testing::StrictMock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::_;

namespace {

class MockMessageCodec : public IMessageCodec {
public:
    MOCK_METHOD(std::vector<uint8_t>, encode,
                (std::shared_ptr<IProcessMessage>), (override));
    MOCK_METHOD(std::shared_ptr<IProcessMessage>, decode,
                (const std::vector<uint8_t>&), (override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info,  (const std::string&), (override));
    MOCK_METHOD(void, warn,  (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
};

std::string unique_name(const std::string& base) {
    return "/" + base + std::to_string(::getpid()) + std::to_string(::time(nullptr));
}

} // namespace

// 正常系: メッセージがキューに送信され、同じ内容で取得できる
TEST(ProcessSenderIntegration, SendNormal_PushesAndPopsMessage) {
    mq_stub_reset();

    NiceMock<MockLogger> logger;
    StrictMock<MockMessageCodec> codec;
    auto queue = std::make_shared<ProcessQueue>(
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
        std::shared_ptr<IMessageCodec>(&codec, [](IMessageCodec*){}),
        unique_name("sender_normal"));

    auto msg = std::make_shared<ProcessMessage>(
        ProcessMessageType::StartBuzzing, std::vector<std::string>{"1"});
    auto imsg = std::static_pointer_cast<IProcessMessage>(msg);

    EXPECT_CALL(codec, encode(imsg))
        .WillOnce([](std::shared_ptr<IProcessMessage> m){
            auto s = m->to_string();
            return std::vector<uint8_t>(s.begin(), s.end());
        });
    EXPECT_CALL(codec, decode(_))
        .WillOnce(Return(imsg));

    ProcessSender sender(queue, msg);
    sender.send();

    auto popped = queue->pop();
    ASSERT_NE(popped, nullptr);
    EXPECT_EQ(popped->type(), msg->type());
    EXPECT_EQ(popped->payload(), msg->payload());
}

// 異常系: nullptr メッセージは送信されず、キューサイズは 0
TEST(ProcessSenderIntegration, SendAbnormal_NullMessage_NoPush) {
    mq_stub_reset();

    NiceMock<MockLogger> logger;
    StrictMock<MockMessageCodec> codec;
    auto queue = std::make_shared<ProcessQueue>(
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
        std::shared_ptr<IMessageCodec>(&codec, [](IMessageCodec*){}),
        unique_name("sender_abnormal"));

    EXPECT_CALL(codec, encode(::testing::_)).Times(0);

    ProcessSender sender(queue, nullptr);
    sender.send();

    EXPECT_EQ(queue->size(), 0u);
}

