#pragma once
#include "infra/thread_message_operation/i_message_queue.hpp"
#include "infra/logger/i_logger.hpp"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <memory>

namespace device_reminder {

class ThreadMessageQueue : public IThreadMessageQueue {
public:
    explicit ThreadMessageQueue(std::shared_ptr<ILogger> logger = nullptr);
    ~ThreadMessageQueue() override = default;

    bool push(const ThreadMessage& msg) override;
    std::optional<ThreadMessage> pop() override;
    bool pop(ThreadMessage& out) override;

    bool is_open() const noexcept override;
    void close() override;

private:
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    std::queue<ThreadMessage> q_;
    bool open_{true};
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder
