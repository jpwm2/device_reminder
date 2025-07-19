#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>

namespace device_reminder {

template <typename T>
class TSQueue {
public:
    bool push(const T& value) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (closed_) return false;
        q_.push(value);
        cv_.notify_one();
        return true;
    }

    bool pop_wait(T& out) {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this]{ return closed_ || !q_.empty(); });
        if (q_.empty()) return false;
        out = q_.front();
        q_.pop();
        return true;
    }

    void shutdown() {
        std::lock_guard<std::mutex> lock(mtx_);
        closed_ = true;
        cv_.notify_all();
    }

private:
    std::queue<T>      q_;
    std::mutex         mtx_;
    std::condition_variable cv_;
    bool               closed_{false};
};

} // namespace device_reminder
