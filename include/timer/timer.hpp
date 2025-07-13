#pragma once
#include <functional>
#include <pthread.h>
#include <atomic>

namespace device_reminder {

class Timer {
public:
    using TimeoutCallback = std::function<void()>;

    Timer(uint32_t duration_ms, TimeoutCallback callback);
    ~Timer();

    void start();
    void stop();

private:
    static void* thread_entry(void* arg);
    void run();

    uint32_t duration_ms_;
    TimeoutCallback callback_;
    pthread_t thread_;
    std::atomic<bool> active_;
};

} // namespace device_reminder