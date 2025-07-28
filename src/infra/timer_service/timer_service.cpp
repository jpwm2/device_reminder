#include "timer_service/timer_service.hpp"
#include "infra/logger/i_logger.hpp"

#include <chrono>
#include <thread>

namespace device_reminder {

TimerService::TimerService(std::shared_ptr<ILogger> logger,
                           int duration_ms,
                           std::shared_ptr<IThreadSender> sender)
    : logger_(std::move(logger)),
      duration_ms_(duration_ms),
      sender_(std::move(sender)) {
    if (logger_) logger_->info("TimerService created");
}

TimerService::~TimerService() {
    stop();
    if (logger_) logger_->info("TimerService destroyed");
}

void TimerService::start() {
    stop();
    running_ = true;
    thread_ = std::thread(&TimerService::worker, this);
    if (logger_) logger_->info("TimerService started");
}

void TimerService::stop() {
    running_ = false;
    if (thread_.joinable()) thread_.join();
    if (logger_) logger_->info("TimerService stopped");
}

void TimerService::worker() {
    std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms_));
    if (running_ && sender_) {
        sender_->send();
        if (logger_) logger_->info("TimerService timeout");
    }
    running_ = false;
}

} // namespace device_reminder
