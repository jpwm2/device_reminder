#include "thread_message_operation/thread_message_queue.hpp"
#include "infra/logger/i_logger.hpp"

namespace device_reminder {

ThreadMessageQueue::ThreadMessageQueue(std::shared_ptr<ILogger> logger)
    : logger_(std::move(logger)) {}

bool ThreadMessageQueue::push(const ThreadMessage& msg) {
    std::lock_guard lk{mtx_};
    if (!open_) return false;
    q_.push(msg);
    cv_.notify_one();
    return true;
}

std::optional<ThreadMessage> ThreadMessageQueue::pop() {
    ThreadMessage out{};
    if (!pop(out)) return std::nullopt;
    return out;
}

bool ThreadMessageQueue::pop(ThreadMessage& out) {
    std::unique_lock lk{mtx_};
    cv_.wait(lk, [this]{ return !q_.empty() || !open_; });
    if (q_.empty() && !open_) return false;
    out = q_.front();
    q_.pop();
    return true;
}

bool ThreadMessageQueue::is_open() const noexcept {
    std::lock_guard lk{mtx_};
    return open_;
}

void ThreadMessageQueue::close() {
    {
        std::lock_guard lk{mtx_};
        open_ = false;
    }
    cv_.notify_all();
}

} // namespace device_reminder
