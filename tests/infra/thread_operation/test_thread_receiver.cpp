#include <gtest/gtest.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "infra/thread_operation/thread_receiver/thread_receiver.hpp"
#include "infra/thread_operation/thread_queue/thread_queue.hpp"
#include "infra/thread_operation/thread_dispatcher/i_thread_dispatcher.hpp"

using namespace device_reminder;

namespace {
class MockDispatcher : public IThreadDispatcher {
public:
    void dispatch(const ThreadMessage& msg) override {
        std::lock_guard lk(m);
        received = msg;
        called = true;
        cv.notify_one();
    }
    ThreadMessage received{};
    bool called{false};
    std::mutex m;
    std::condition_variable cv;
};
} // namespace

TEST(ThreadReceiverTest, DispatchesMessages) {
    auto queue = std::make_shared<ThreadQueue>(nullptr);
    auto dispatcher = std::make_shared<MockDispatcher>();
    ThreadReceiver receiver(queue, dispatcher);

    std::thread th{[&]{ receiver.run(); }};

    ThreadMessage msg{ThreadMessageType::StartBuzzing, true};
    queue->push(msg);

    {
        std::unique_lock lk(dispatcher->m);
        dispatcher->cv.wait_for(lk, std::chrono::milliseconds(100), [&]{ return dispatcher->called; });
    }

    receiver.stop();
    th.join();

    ASSERT_TRUE(dispatcher->called);
    EXPECT_EQ(dispatcher->received.type_, msg.type_);
    EXPECT_EQ(dispatcher->received.payload_, msg.payload_);
}
