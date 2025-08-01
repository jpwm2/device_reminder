#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/process_operation/process_queue/process_queue.hpp"
#include "infra/process_operation/message_codec/message_codec.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/logger/i_logger.hpp"
#include "posix_mq_stub.h"

using namespace device_reminder;
using ::testing::NiceMock;
using ::testing::StrictMock;

namespace {
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};
}

static std::string unique_name(const std::string& base) {
    return "/" + base + std::to_string(::getpid()) + std::to_string(::time(nullptr));
}

TEST(ProcessQueueTest, ConstructorThrowsOnInvalidName) {
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    auto codec = std::make_shared<MessageCodec>(nullptr);
    EXPECT_THROW(ProcessQueue(logger, codec, "invalid"), std::invalid_argument);
}

TEST(ProcessQueueTest, OpenFailureLogsError) {
    mq_stub_reset();
    mq_stub_set_fail_open(1);
    NiceMock<MockLogger> logger;
    auto codec = std::make_shared<MessageCodec>(nullptr);
    EXPECT_CALL(logger, error(testing::HasSubstr("mq_open failed"))).Times(1);
    EXPECT_THROW(ProcessQueue(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), codec, unique_name("fail_")), std::system_error);
}

TEST(ProcessQueueTest, PushAndPopWorks) {
    mq_stub_reset();
    NiceMock<MockLogger> logger;
    auto codec = std::make_shared<MessageCodec>(nullptr);
    auto name = unique_name("ok_");
    ProcessQueue q(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), codec, name);
    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{"1"});
    q.push(msg);
    auto res = q.pop();
    ASSERT_NE(res, nullptr);
    EXPECT_EQ(res->type(), msg->type());
    EXPECT_EQ(res->payload(), msg->payload());
}

TEST(ProcessQueueTest, PushNullMessageDoesNothing) {
    mq_stub_reset();
    auto codec = std::make_shared<MessageCodec>(nullptr);
    ProcessQueue q(nullptr, codec, unique_name("null_"));
    q.push(nullptr);
    EXPECT_EQ(q.size(), 0u);
}

TEST(ProcessQueueTest, PushFailureThrows) {
    mq_stub_reset();
    mq_stub_set_fail_send(1);
    NiceMock<MockLogger> logger;
    auto codec = std::make_shared<MessageCodec>(nullptr);
    ProcessQueue q(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), codec, unique_name("fail_send_"));
    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{"1"});
    EXPECT_CALL(logger, error(testing::HasSubstr("mq_send failed"))).Times(1);
    EXPECT_THROW(q.push(msg), std::system_error);
}

TEST(ProcessQueueTest, PopFailureThrows) {
    mq_stub_reset();
    mq_stub_set_fail_receive(1);
    NiceMock<MockLogger> logger;
    auto codec = std::make_shared<MessageCodec>(nullptr);
    ProcessQueue q(std::shared_ptr<ILogger>(&logger, [](ILogger*){}), codec, unique_name("fail_recv_"));
    EXPECT_CALL(logger, error(testing::HasSubstr("mq_receive failed"))).Times(1);
    EXPECT_THROW(q.pop(), std::system_error);
}
