#include <gtest/gtest.h>

#include "infra/watch_dog/watch_dog.hpp"
#include "infra/timer_service/timer_service.hpp"
#include "infra/message/thread_sender.hpp"
#include "infra/message/message.hpp"

#include <chrono>
#include <thread>
#include <vector>
#include <mutex>

namespace device_reminder {

class TestQueue : public IMessageQueue {
public:
    void push(std::shared_ptr<IMessage> msg) override {
        std::lock_guard<std::mutex> lock(mtx_);
        msgs_.push_back(std::move(msg));
    }
    std::shared_ptr<IMessage> pop() override {
        std::lock_guard<std::mutex> lock(mtx_);
        if (msgs_.empty()) return nullptr;
        auto msg = msgs_.front();
        msgs_.erase(msgs_.begin());
        return msg;
    }
    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return msgs_.size();
    }

private:
    mutable std::mutex mtx_{};
    std::vector<std::shared_ptr<IMessage>> msgs_{};
};

TEST(WatchDogTest, KickRestartsTimerAndSendsMessage) {
    auto timer = std::make_shared<TimerService>(nullptr);
    auto queue = std::make_shared<TestQueue>();
    auto message = std::make_shared<Message>(MessageType::Timeout, std::vector<std::string>{}, nullptr);
    auto sender = std::make_shared<ThreadSender>(nullptr);

    WatchDog wd(timer, queue, message, sender, nullptr, 50);

    wd.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    EXPECT_EQ(queue->size(), 0u);

    wd.kick();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    EXPECT_EQ(queue->size(), 0u);

    std::this_thread::sleep_for(std::chrono::milliseconds(40));
    EXPECT_EQ(queue->size(), 1u);

    wd.stop();
}

} // namespace device_reminder
