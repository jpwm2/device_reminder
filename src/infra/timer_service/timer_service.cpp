#include "infra/timer_service/timer_service.hpp"

#include <chrono>
#include <stdexcept>
#include <thread>

namespace device_reminder {

TimerService::TimerService(std::shared_ptr<ILogger> logger)
    : logger_(std::move(logger)) {}

TimerService::~TimerService() {
    try {
        stop();
    } catch (...) {
        // 破棄時の例外は無視する
    }
}

void TimerService::start(int duration_ms,
                         std::shared_ptr<IThreadSender> sender,
                         std::shared_ptr<IMessageQueue> queue,
                         std::shared_ptr<IMessage> message) {
    if (logger_) {
        logger_->info("TimerService start: duration_ms=" + std::to_string(duration_ms) +
                      ", sender=" + (sender ? std::string("valid") : std::string("null")) +
                      ", queue=" + (queue ? std::string("valid") : std::string("null")) +
                      ", message=" + (message ? std::string("valid") : std::string("null")));
    }

    if (running_) {
        cancel_.store(true);
        if (worker_future_.valid()) {
            worker_future_.get();
        }
        running_.store(false);
        if (logger_) logger_->warn("TimerService start: existing timer stopped");
    }

    if (!sender) {
        if (logger_) logger_->error("TimerService start: sender is null");
        throw std::invalid_argument("sender is null");
    }
    if (!queue) {
        if (logger_) logger_->error("TimerService start: queue is null");
        throw std::invalid_argument("queue is null");
    }
    if (!message) {
        if (logger_) logger_->error("TimerService start: message is null");
        throw std::invalid_argument("message is null");
    }

    sender_ = std::move(sender);
    queue_ = std::move(queue);
    message_ = std::move(message);
    cancel_.store(false);
    running_.store(true);

    try {
        worker_future_ = std::async(std::launch::async, [this, duration_ms] {
            try {
                worker(duration_ms);
            } catch (...) {
                if (logger_) logger_->error("TimerService worker: exception");
                throw;
            }
        });
        if (worker_future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            worker_future_.get();
        }
        if (logger_) logger_->info("TimerService start success");
    } catch (...) {
        running_.store(false);
        if (logger_) logger_->error("TimerService start failed");
        throw;
    }
}

void TimerService::stop() {
    if (logger_) logger_->info("TimerService stop start");

    if (!running_) {
        if (logger_) logger_->info("TimerService stop: no timer running");
        return;
    }

    cancel_.store(true);
    try {
        if (worker_future_.valid()) {
            worker_future_.get();
        }
        running_.store(false);

        sender_.reset();
        queue_.reset();
        message_.reset();
        worker_future_ = std::future<void>();
        if (logger_) logger_->info("TimerService stop success");
    } catch (...) {
        if (logger_) logger_->error("TimerService stop failed");
        running_.store(false);
        sender_.reset();
        queue_.reset();
        message_.reset();
        worker_future_ = std::future<void>();
        throw;
    }
}

void TimerService::worker(int duration_ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
    if (cancel_.load()) {
        if (logger_) logger_->info("TimerService worker: canceled");
    } else {
        sender_->send(queue_, message_);
        if (logger_) logger_->info("TimerService worker: send success");
    }
    running_.store(false);
}

} // namespace device_reminder
