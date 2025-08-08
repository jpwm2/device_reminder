#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "infra/logger/i_logger.hpp"
#include "infra/message/i_message.hpp"
#include "infra/process_operation/process_dispatcher/i_process_dispatcher.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/process_operation/process_receiver/process_receiver.hpp"

#include <thread>

using namespace device_reminder;
using ::testing::AtLeast;
using ::testing::NiceMock;
using ::testing::Return;

namespace {

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
};

class MockQueue : public IProcessQueue {
public:
    MOCK_METHOD(void, push, (std::shared_ptr<IMessage>), (override));
    MOCK_METHOD(std::shared_ptr<IMessage>, pop, (), (override));
};

class MockDispatcher : public IProcessDispatcher {
public:
    MOCK_METHOD(void, dispatch, (std::shared_ptr<IMessage>), (override));
};

class DummyMessage : public IMessage {
public:
    MessageType type() const noexcept override { return MessageType::None; }
    std::vector<std::string> payload() const noexcept override { return {}; }
    std::string to_string() const override { return {}; }
};

} // namespace

TEST(ProcessReceiverTest, DispatchesMessage) {
    auto msg = std::make_shared<DummyMessage>();

    NiceMock<MockLogger> logger;
    MockQueue queue;
    MockDispatcher dispatcher;

    EXPECT_CALL(logger, info("受信スレッド開始")).Times(1);
    EXPECT_CALL(logger, info("メッセージをディスパッチしました")).Times(1);
    EXPECT_CALL(logger, info("受信スレッド停止")).Times(1);
    EXPECT_CALL(logger, info("受信スレッド停止完了")).Times(1);

    EXPECT_CALL(queue, pop())
        .WillOnce(Return(msg))
        .WillRepeatedly(Return(nullptr));
    auto msg_base = std::static_pointer_cast<IMessage>(msg);
    EXPECT_CALL(dispatcher, dispatch(msg_base)).Times(1);

    ProcessReceiver receiver(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                            std::shared_ptr<IProcessQueue>(&queue, [](IProcessQueue*){}),
                            std::shared_ptr<IProcessDispatcher>(&dispatcher, [](IProcessDispatcher*){}));
    receiver.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    receiver.stop();
}

TEST(ProcessReceiverTest, NullMessageLogsWarning) {
    NiceMock<MockLogger> logger;
    MockQueue queue;
    MockDispatcher dispatcher;

    EXPECT_CALL(logger, info("受信スレッド開始")).Times(1);
    EXPECT_CALL(logger, warn("受信メッセージがnullptrのため無視")).Times(AtLeast(1));
    EXPECT_CALL(logger, info("受信スレッド停止")).Times(1);
    EXPECT_CALL(logger, info("受信スレッド停止完了")).Times(1);

    EXPECT_CALL(queue, pop()).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(dispatcher, dispatch(::testing::_)).Times(0);

    ProcessReceiver receiver(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                            std::shared_ptr<IProcessQueue>(&queue, [](IProcessQueue*){}),
                            std::shared_ptr<IProcessDispatcher>(&dispatcher, [](IProcessDispatcher*){}));
    receiver.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    receiver.stop();
}

