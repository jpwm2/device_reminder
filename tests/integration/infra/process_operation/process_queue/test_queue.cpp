#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/logger.h>

#include "infra/process_operation/process_queue/process_queue.hpp"
#include "infra/process_operation/message_codec/message_codec.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/logger/logger.hpp"

#include "posix_mq_stub.h"

using namespace device_reminder;
using ::testing::NiceMock;

namespace {
class MockSink : public spdlog::sinks::sink {
public:
    MOCK_METHOD(void, log, (const spdlog::details::log_msg& msg), (override));
    MOCK_METHOD(void, flush, (), (override));
    MOCK_METHOD(void, set_pattern, (const std::string&), (override));
    MOCK_METHOD(void, set_formatter, (std::unique_ptr<spdlog::formatter>), (override));
};

std::string unique_name(const std::string& base) {
    return "/" + base + std::to_string(::getpid()) + std::to_string(::time(nullptr));
}
} // namespace

TEST(ProcessQueueIntegrationTest, PushPopNormal) {
    mq_stub_reset();
    auto sink = std::make_shared<spdlog::sinks::null_sink_mt>();
    auto spd = std::make_shared<spdlog::logger>("test", sink);
    auto logger = std::make_shared<Logger>(spd);
    auto codec  = std::make_shared<MessageCodec>(logger);
    ProcessQueue q(logger, codec, unique_name("ok"));

    auto msg = std::make_shared<ProcessMessage>(
        ProcessMessageType::StartBuzzing, std::vector<std::string>{"1"});

    q.push(msg);
    auto res = q.pop();

    ASSERT_NE(res, nullptr);
    EXPECT_EQ(res->type(), msg->type());
    EXPECT_EQ(res->payload(), msg->payload());
}

TEST(ProcessQueueIntegrationTest, PushFailureLogged) {
    mq_stub_reset();
    mq_stub_set_fail_send(1);

    auto sink = std::make_shared<NiceMock<MockSink>>();
    auto spd = std::make_shared<spdlog::logger>("test", sink);
    auto logger = std::make_shared<Logger>(spd);
    auto codec  = std::make_shared<MessageCodec>(logger);
    ProcessQueue q(logger, codec, unique_name("fail"));

    auto msg = std::make_shared<ProcessMessage>(
        ProcessMessageType::StartBuzzing, std::vector<std::string>{"1"});

    std::vector<std::string> logs;
    ON_CALL(*sink, log(testing::_))
        .WillByDefault([&logs](const spdlog::details::log_msg& m) {
            logs.emplace_back(m.payload.begin(), m.payload.end());
        });

    EXPECT_THROW(q.push(msg), std::system_error);

    bool found = std::any_of(logs.begin(), logs.end(), [](const std::string& s) {
        return s.find("mq_send failed") != std::string::npos;
    });
    EXPECT_TRUE(found);
}

