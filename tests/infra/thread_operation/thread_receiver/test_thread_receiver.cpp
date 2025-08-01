#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <vector>
#include "infra/thread_operation/thread_message/thread_message.hpp"
#include "infra/thread_operation/thread_receiver/thread_receiver.hpp"
#include "infra/thread_operation/thread_queue/thread_queue.hpp"
#include "infra/thread_operation/thread_queue/i_thread_queue.hpp"
#include "infra/thread_operation/thread_dispatcher/i_thread_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"

using namespace device_reminder;
using ::testing::StrictMock;

namespace {
class RecordingDispatcher : public IThreadDispatcher {
public:
    void dispatch(std::shared_ptr<IThreadMessage> msg) override {
        std::lock_guard lk(m);
        received = std::move(msg);
        called = true;
        cv.notify_one();
    }
    std::shared_ptr<IThreadMessage> received{nullptr};
    bool called{false};
    std::mutex m;
    std::condition_variable cv;
};
} // namespace

namespace {
class MockQueue : public IThreadQueue {
public:
    MOCK_METHOD(void, push, (std::shared_ptr<IThreadMessage>), (override));
    MOCK_METHOD(std::shared_ptr<IThreadMessage>, pop, (), (override));
    MOCK_METHOD(size_t, size, (), (const, override));
};

class MockDispatcher : public IThreadDispatcher {
public:
    MOCK_METHOD(void, dispatch, (std::shared_ptr<IThreadMessage>), (override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};
} // namespace

TEST(ThreadReceiverTest, DispatchesMessages) {
    auto queue = std::make_shared<ThreadQueue>(nullptr);
    auto dispatcher = std::make_shared<RecordingDispatcher>();
    ThreadReceiver receiver(queue, dispatcher);

    std::thread th{[&]{ receiver.run(); }};

    auto message = std::make_shared<ThreadMessage>(
        ThreadMessageType::StartBuzzer, std::vector<std::string>{"1"});
    queue->push(message);

    {
        std::unique_lock lk(dispatcher->m);
        dispatcher->cv.wait_for(lk, std::chrono::milliseconds(100), [&]{ return dispatcher->called; });
    }

    receiver.stop();
    th.join();

    ASSERT_TRUE(dispatcher->called);
    ASSERT_NE(dispatcher->received, nullptr);
    EXPECT_EQ(dispatcher->received->type(), message->type());
    EXPECT_EQ(dispatcher->received->payload(), message->payload());
}

TEST(ThreadReceiverTest, ConstructorLogsCreation) {
    StrictMock<MockLogger> logger;
    auto queue = std::make_shared<ThreadQueue>(nullptr);
    auto dispatcher = std::make_shared<RecordingDispatcher>();

    EXPECT_CALL(logger, info("ThreadReceiver created")).Times(1);

    ThreadReceiver receiver(queue, dispatcher,
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}));
}

TEST(ThreadReceiverTest, RunEndsWhenQueueIsNull) {
    StrictMock<MockLogger> logger;
    EXPECT_CALL(logger, info("ThreadReceiver created")).Times(1);
    EXPECT_CALL(logger, info("ThreadReceiver loop end")).Times(1);

    ThreadReceiver receiver(nullptr, std::make_shared<RecordingDispatcher>(),
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}));

    receiver.run();
}

TEST(ThreadReceiverTest, RunNoDispatchWhenDispatcherNull) {
    StrictMock<MockLogger> logger;
    auto queue = std::make_shared<ThreadQueue>(nullptr);
    queue->push(std::make_shared<ThreadMessage>(
        ThreadMessageType::StartBuzzer, std::vector<std::string>{"1"}));

    EXPECT_CALL(logger, info("ThreadReceiver created")).Times(1);
    EXPECT_CALL(logger, info("ThreadReceiver loop end")).Times(1);

    ThreadReceiver receiver(queue, nullptr,
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}));

    receiver.run();
    EXPECT_EQ(queue->size(), 0u);
}

TEST(ThreadReceiverTest, RunAfterStopImmediatelyEnds) {
    StrictMock<MockLogger> logger;
    auto queue = std::make_shared<ThreadQueue>(nullptr);
    auto dispatcher = std::make_shared<RecordingDispatcher>();

    {
        testing::InSequence seq;
        EXPECT_CALL(logger, info("ThreadReceiver created")).Times(1);
        EXPECT_CALL(logger, info("ThreadReceiver stop requested")).Times(1);
        EXPECT_CALL(logger, info("ThreadReceiver loop end")).Times(1);
    }

    ThreadReceiver receiver(queue, dispatcher,
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}));

    receiver.stop();
    receiver.run();
}

TEST(ThreadReceiverTest, DispatcherExceptionPropagates) {
    auto queue = std::make_shared<ThreadQueue>(nullptr);
    queue->push(std::make_shared<ThreadMessage>(
        ThreadMessageType::StartBuzzer, std::vector<std::string>{"1"}));
    StrictMock<MockDispatcher> dispatcher;
    EXPECT_CALL(dispatcher, dispatch(testing::_))
        .WillOnce(testing::Throw(std::runtime_error("boom")));

    ThreadReceiver receiver(queue,
        std::shared_ptr<IThreadDispatcher>(&dispatcher, [](IThreadDispatcher*){}),
        nullptr);

    EXPECT_THROW(receiver.run(), std::runtime_error);
}

TEST(ThreadReceiverTest, QueueReturningNullEndsLoop) {
    StrictMock<MockQueue> queue;
    StrictMock<MockLogger> logger;
    StrictMock<MockDispatcher> dispatcher;

    EXPECT_CALL(logger, info("ThreadReceiver created")).Times(1);
    EXPECT_CALL(queue, pop()).WillOnce(testing::Return(nullptr));
    EXPECT_CALL(dispatcher, dispatch(testing::_)).Times(0);
    EXPECT_CALL(logger, info("ThreadReceiver loop end")).Times(1);

    ThreadReceiver receiver(
        std::shared_ptr<IThreadQueue>(&queue, [](IThreadQueue*){}),
        std::shared_ptr<IThreadDispatcher>(&dispatcher, [](IThreadDispatcher*){}),
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}));

    receiver.run();
}
