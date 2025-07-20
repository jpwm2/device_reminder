#include "timer_service/timer_service.hpp"

#include <sys/timerfd.h>
#include <poll.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

namespace device_reminder {
namespace {

/// ワンショット timerfd を生成
int create_one_shot_timer_fd(uint32_t ms) {
    int fd = ::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
    if (fd == -1) return -1;

    itimerspec spec{};
    spec.it_value.tv_sec  = ms / 1000;
    spec.it_value.tv_nsec = (ms % 1000) * 1'000'000ULL;  // ns へ変換

    if (::timerfd_settime(fd, 0, &spec, nullptr) == -1) {
        ::close(fd);
        return -1;
    }
    return fd;
}

} // unnamed namespace

TimerService::TimerService(std::shared_ptr<IMessageSender> sender)
    : sender_(std::move(sender)) {}

TimerService::~TimerService() {
    stop();
}

void TimerService::start(uint32_t ms, const Message& timeout_msg) {
    stop();                         // 既存タイマーを殺してから再起動
    running_ = true;
    active_  = true;
    thread_  = std::thread(&TimerService::worker, this, ms, timeout_msg);
}

void TimerService::stop() {
    running_ = false;
    if (thread_.joinable()) thread_.join();
    active_ = false;
}

void TimerService::worker(uint32_t ms, Message timeout_msg) {
    int tfd = create_one_shot_timer_fd(ms);
    if (tfd == -1) {
        // 生成失敗時は即座にメッセージ送出して終了
        if (sender_) sender_->enqueue(timeout_msg);
        active_ = false;
        return;
    }

    pollfd pfd{ tfd, POLLIN, 0 };
    while (running_) {
        int ret = ::poll(&pfd, 1, -1);         // 無限待機
        if (ret == -1) {
            if (errno == EINTR) continue;      // シグナル割り込みは再待機
            break;
        }
        if (pfd.revents & POLLIN) {
            uint64_t expirations;
            ::read(tfd, &expirations, sizeof(expirations));  // カウンタ読み捨て
            if (sender_) sender_->enqueue(timeout_msg);      // タイムアウト通知
            break;
        }
    }
    ::close(tfd);
    active_ = false;
}

} // namespace device_reminder
