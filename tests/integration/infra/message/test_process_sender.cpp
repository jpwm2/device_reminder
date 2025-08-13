#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/message/process_sender.hpp"
#include "infra/message/message_queue.hpp"
#include "infra/message/message_queue.hpp"
#include "infra/message/message.hpp"
#include "infra/message/message_codec.hpp"
#include "infra/logger.hpp"
#include "posix_mq_stub.h"

using namespace device_reminder;
using ::testing::StrictMock;
using ::testing::Return;
using ::testing::Throw;

namespace {

class MockQueue : public IProcessQueue {
public:
    MOCK_METHOD(void, push, (std::shared_ptr<IProcessMessage>), (override));
    MOCK_METHOD(std::shared_ptr<IProcessMessage>, pop, (), (override));
    MOCK_METHOD(std::size_t, size, (), (const, override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

std::string unique_name(const std::string& base) {
    return "/" + base + std::to_string(::getpid()) + std::to_string(::time(nullptr));
}

} // namespace

TEST(ProcessSenderTest, ValueNormal_EnqueuesMessage) {
    mq_stub_reset();
    std::shared_ptr<ILogger> logger{};
    auto codec = std::make_shared<MessageCodec>(nullptr);
    auto queue = std::make_shared<ProcessQueue>(logger, codec, unique_name("sender1_"));
    std::shared_ptr<IProcessMessage> msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{"1"});
    ProcessSender sender(queue, msg);

    sender.send();

    auto popped = queue->pop();
    ASSERT_NE(popped, nullptr);
    EXPECT_EQ(popped->type(), msg->type());
    EXPECT_EQ(popped->payload(), msg->payload());
}

TEST(ProcessSenderTest, ValueAbnormal_LongPayload) {
    mq_stub_reset();
    std::shared_ptr<ILogger> logger{};
    auto codec = std::make_shared<MessageCodec>(nullptr);
    auto queue = std::make_shared<ProcessQueue>(logger, codec, unique_name("sender2_"));
    std::string long_str(1024, 'x');
    std::shared_ptr<IProcessMessage> msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{long_str});
    ProcessSender sender(queue, msg);
    EXPECT_NO_THROW(sender.send());
}

TEST(ProcessSenderTest, PointerNormal_NullQueue) {
    std::shared_ptr<IProcessMessage> msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{});
    ProcessSender sender(nullptr, msg);
    EXPECT_NO_THROW(sender.send());
}

TEST(ProcessSenderTest, PointerAbnormal_NullMessage) {
    StrictMock<MockQueue> queue;
    EXPECT_CALL(queue, push(::testing::IsNull())).Times(1);
    ProcessSender sender(std::shared_ptr<IProcessQueue>(&queue, [](IProcessQueue*){}), nullptr);
    sender.send();
}

TEST(ProcessSenderTest, PointerAbnormal_BothNull) {
    ProcessSender sender(nullptr, nullptr);
    EXPECT_NO_THROW(sender.send());
}

TEST(ProcessSenderTest, MockNormal_PushCalledOnce) {
    StrictMock<MockQueue> queue;
    std::shared_ptr<IProcessMessage> msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{});
    EXPECT_CALL(queue, push(msg)).Times(1);
    ProcessSender sender(std::shared_ptr<IProcessQueue>(&queue, [](IProcessQueue*){}), msg);
    sender.send();
}

TEST(ProcessSenderTest, MockAbnormal_ThrowsFromQueue) {
    StrictMock<MockQueue> queue;
    std::shared_ptr<IProcessMessage> msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{});
    EXPECT_CALL(queue, push(msg)).WillOnce(Throw(std::runtime_error("fail")));
    ProcessSender sender(std::shared_ptr<IProcessQueue>(&queue, [](IProcessQueue*){}), msg);
    EXPECT_THROW(sender.send(), std::runtime_error);
}
