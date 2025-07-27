#include <gtest/gtest.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "thread_message_operation/thread_message_receiver.hpp"
#include "thread_message_operation/thread_message_queue.hpp"

using namespace device_reminder;

TEST(ThreadMessageReceiverTest, ReceivesMessages) {
    auto queue = std::make_shared<ThreadMessageQueue>();
    ThreadMessage received{};
    std::mutex m; std::condition_variable cv; bool got = false;

    ThreadMessageReceiver receiver(queue,
        [&](const ThreadMessage& msg){
            std::lock_guard lk(m);
            received = msg;
            got = true;
            cv.notify_one();
        });

    std::thread th{std::ref(receiver)};

    ThreadMessage msg{ThreadMessageType::StartBuzzer, true};
    queue->push(msg);

    {
        std::unique_lock lk(m);
        cv.wait_for(lk, std::chrono::milliseconds(100), [&]{ return got; });
    }

    receiver.stop();
    th.join();

    ASSERT_TRUE(got);
    EXPECT_EQ(received.type_, msg.type_);
    EXPECT_EQ(received.payload_, msg.payload_);
}
