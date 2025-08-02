#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>
#include "infra/thread_operation/thread_receiver/thread_receiver.hpp"
#include "infra/thread_operation/thread_queue/thread_queue.hpp"
#include "infra/thread_operation/thread_dispatcher/thread_dispatcher.hpp"
#include "infra/thread_operation/thread_message/thread_message.hpp"
#include "infra/logger/i_logger.hpp"

using namespace device_reminder;

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

TEST(ThreadReceiverIntegrationTest, DispatchesValidMessage) {
    auto logger = std::make_shared<testing::NiceMock<MockLogger>>();
    EXPECT_CALL(*logger, info(testing::_)).Times(testing::AnyNumber());

    auto queue = std::make_shared<ThreadQueue>(logger);

    std::mutex m;
    std::condition_variable cv;
    bool called = false;
    std::shared_ptr<IThreadMessage> received;

    ThreadDispatcher::HandlerMap handler_map;
    handler_map[ThreadMessageType::StartBuzzing] = [&](std::shared_ptr<IThreadMessage> msg) {
        {
            std::lock_guard<std::mutex> lk(m);
            called = true;
            received = std::move(msg);
        }
        cv.notify_one();
    };

    auto dispatcher = std::make_shared<ThreadDispatcher>(logger, handler_map);
    ThreadReceiver receiver(queue, dispatcher, logger);

    auto msg = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing, std::vector<std::string>{"1"});
    queue->push(msg);

    std::thread th([&] { receiver.run(); });

    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait_for(lk, std::chrono::milliseconds(100), [&] { return called; });
    }

    receiver.stop();
    th.join();

    ASSERT_TRUE(called);
    ASSERT_NE(received, nullptr);
    EXPECT_EQ(received->type(), ThreadMessageType::StartBuzzing);
    EXPECT_EQ(received->payload(), std::vector<std::string>{"1"});
    EXPECT_EQ(queue->size(), 0u);
}

TEST(ThreadReceiverIntegrationTest, LogsUnhandledMessage) {
    auto logger = std::make_shared<testing::NiceMock<MockLogger>>();
    EXPECT_CALL(*logger, info(testing::_)).Times(testing::AnyNumber());
    EXPECT_CALL(*logger, info("Unhandled thread message")).Times(1);

    auto queue = std::make_shared<ThreadQueue>(logger);

    bool called = false;
    ThreadDispatcher::HandlerMap handler_map;
    handler_map[ThreadMessageType::StartBuzzing] = [&](std::shared_ptr<IThreadMessage>) {
        called = true;
    };

    auto dispatcher = std::make_shared<ThreadDispatcher>(logger, handler_map);
    ThreadReceiver receiver(queue, dispatcher, logger);

    auto msg = std::make_shared<ThreadMessage>(ThreadMessageType::Unknown, std::vector<std::string>{});
    queue->push(msg);

    std::thread th([&] { receiver.run(); });
    th.join();
    receiver.stop();

    EXPECT_FALSE(called);
}

