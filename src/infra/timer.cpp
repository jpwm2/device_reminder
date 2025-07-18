#include "timer.hpp"
#include <unistd.h>

namespace device_reminder {

Timer::Timer(uint32_t duration_ms, TimeoutCallback callback)
    : duration_ms_(duration_ms),
      callback_(std::move(callback)),
      active_(false),
      stop_requested_(false) {}

Timer::~Timer() {
    stop();
}

void Timer::start() {
    if (active_) return;

    stop_requested_ = false;
    active_ = true;

    if (pthread_create(&thread_, nullptr, Timer::thread_entry, this) != 0) {
        active_ = false;
        throw std::runtime_error("Failed to create timer thread");
    }
}

void Timer::stop() {
    if (!active_) return;

    stop_requested_ = true;

    // スレッドの終了を待つ
    pthread_join(thread_, nullptr);
    active_ = false;
}

void* Timer::thread_entry(void* arg) {
    static_cast<Timer*>(arg)->run();
    return nullptr;
}

void Timer::run() {
    // 時間待ち（ms → us）
    usleep(duration_ms_ * 1000);

    if (!stop_requested_ && callback_) {
        callback_();
    }
}

} // namespace device_reminder
