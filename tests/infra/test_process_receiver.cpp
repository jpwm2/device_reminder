#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/process_operation/process_receiver/process_receiver.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/process_operation/process_dispatcher/i_process_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"
#include <thread>

using namespace device_reminder;

namespace {
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

class MockQueue : public IProcessQueue {
public:
    MOCK_METHOD(void, push, (std::shared_ptr<IProcessMessage>), (override));
    MOCK_METHOD(std::shared_ptr<IProcessMessage>, pop, (), (override));
    MOCK_METHOD(std::size_t, size, (), (const, override));
};

class MockDispatcher : public IProcessDispatcher {
public:
    MOCK_METHOD(void, dispatch, (std::shared_ptr<IProcessMessage>), (override));
};
} // namespace

TEST(ProcessReceiverTest, DispatchesMessage) {
    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing, std::vector<std::string>{});

    MockQueue queue;
    MockDispatcher dispatcher;

    EXPECT_CALL(queue, pop())
        .WillOnce(testing::Return(msg))
        .WillRepeatedly(testing::Return(nullptr));
    EXPECT_CALL(dispatcher, dispatch(msg)).Times(1);

    ProcessReceiver receiver(nullptr,
                            std::shared_ptr<IProcessQueue>(&queue, [](IProcessQueue*){}),
                            std::shared_ptr<IProcessDispatcher>(&dispatcher, [](IProcessDispatcher*){}));
    receiver.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    receiver.stop();
}
