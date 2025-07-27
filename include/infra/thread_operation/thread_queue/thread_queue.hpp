#pragma once
#include "infra/thread_operation/thread_queue/i_thread_queue.hpp"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <chrono>

namespace device_reminder {

template <typename T>
class ThreadQueue : public IThreadQueue<T> {
public:
    ThreadQueue() = default;

    void push(const T& item) override {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            q_.push(item);
        }
        cv_.notify_one();
    }

    std::optional<T> pop(int timeout_ms = -1) override {
        std::unique_lock<std::mutex> lock(mtx_);
        if (timeout_ms < 0) {
            cv_.wait(lock, [this]{ return !q_.empty(); });
        } else {
            if (!cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms), [this]{ return !q_.empty(); })) {
                return std::nullopt;
            }
        }
        T val = q_.front();
        q_.pop();
        return val;
    }

    bool empty() const override {
        std::lock_guard<std::mutex> lock(mtx_);
        return q_.empty();
    }

    size_t size() const override {
        std::lock_guard<std::mutex> lock(mtx_);
        return q_.size();
    }

    void clear() override {
        std::lock_guard<std::mutex> lock(mtx_);
        std::queue<T> empty_q;
        std::swap(q_, empty_q);
    }

private:
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    std::queue<T> q_;
};

} // namespace device_reminder
