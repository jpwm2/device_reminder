#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/worker_dispatcher/worker_dispatcher.hpp"
#include "thread_message_operation/thread_message_queue.hpp"

using namespace device_reminder;

TEST(WorkerDispatcherTest, DispatchesMessage) {
    auto queue = std::make_shared<ThreadMessageQueue>();
    ThreadMessage received{};
    std::mutex m;
    std::condition_variable cv;
    bool got = false;
    auto handler = [&](const ThreadMessage& msg){
        std::lock_guard lk(m);
        received = msg;
        got = true;
        cv.notify_one();
    };

    WorkerDispatcher disp(queue, handler);
    disp.start();

    ThreadMessage msg{MessageType::BuzzerOn, true};
    queue->push(msg);

    {
        std::unique_lock lk(m);
        cv.wait_for(lk, std::chrono::milliseconds(100), [&]{ return got; });
    }

    disp.stop();
    disp.join();
    ASSERT_TRUE(got);
    EXPECT_EQ(received.type_, msg.type_);
}
