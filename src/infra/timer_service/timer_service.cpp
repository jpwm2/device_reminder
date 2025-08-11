#include "infra/timer_service/timer_service.hpp"

#include <chrono>
#include <stdexcept>

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

void TimerService::start(int duration_ms, std::shared_ptr<IThreadSender> sender) {
    if (logger_) {
        logger_->info("TimerService start: duration_ms=" + std::to_string(duration_ms) +
                      ", sender=" + (sender ? std::string("valid") : std::string("null")));
    }

    if (running_) {
        cancel_.store(true);
        if (thread_.joinable()) {
            thread_.join();
        }
        running_.store(false);
        if (logger_) logger_->warn("TimerService start: existing timer stopped");
    }

    if (!sender) {
        if (logger_) logger_->error("TimerService start: sender is null");
        throw std::invalid_argument("sender is null");
    }

    sender_ = std::move(sender);
    cancel_.store(false);
    running_.store(true);
    worker_exception_ = nullptr;

    try {
        thread_ = std::thread(&TimerService::worker, this, duration_ms);
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
        if (thread_.joinable()) {
            thread_.join();
        }
        running_.store(false);

        if (worker_exception_) {
            if (logger_) logger_->error("TimerService stop: worker exception");
            auto ex = worker_exception_;
            worker_exception_ = nullptr;
            throw;
        }

        sender_.reset();
        thread_ = std::thread();
        if (logger_) logger_->info("TimerService stop success");
    } catch (...) {
        if (logger_) logger_->error("TimerService stop failed");
        running_.store(false);
        sender_.reset();
        thread_ = std::thread();
        throw;
    }
}

void TimerService::worker(int duration_ms) {
    try {
        std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
        if (cancel_.load()) {
            if (logger_) logger_->info("TimerService worker: canceled");
        } else {
            sender_->send();
            if (logger_) logger_->info("TimerService worker: send success");
        }
    } catch (...) {
        worker_exception_ = std::current_exception();
        if (logger_) logger_->error("TimerService worker: exception");
    }
    running_.store(false);
}

} // namespace device_reminder
