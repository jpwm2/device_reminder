#include "watch_dog/watch_dog.hpp"

#include <string>
#include <utility>

namespace device_reminder {

WatchDog::WatchDog(std::shared_ptr<ITimerService> timer_service,
                   std::shared_ptr<IMessageQueue> message_queue,
                   std::shared_ptr<IMessage> message,
                   std::shared_ptr<ILogger> logger)
    : timer_service_(std::move(timer_service))
    , message_queue_(std::move(message_queue))
    , message_(std::move(message))
    , logger_(std::move(logger)) {}

void WatchDog::start() {
    if (logger_) {
        logger_->info("WatchDog start called");
    }
    try {
        timer_service_->start(message_queue_, message_);
        if (logger_) {
            logger_->info("WatchDog start succeeded");
        }
    } catch (...) {
        if (logger_) {
            try {
                throw;
            } catch (const std::exception& e) {
                logger_->error(std::string("WatchDog start failed: ") + e.what());
            } catch (...) {
                logger_->error("WatchDog start failed: unknown error");
            }
        }
        throw;
    }
}

void WatchDog::stop() {
    if (logger_) {
        logger_->info("WatchDog stop called");
    }
    try {
        timer_service_->stop();
        if (logger_) {
            logger_->info("WatchDog stop succeeded");
        }
    } catch (...) {
        if (logger_) {
            try {
                throw;
            } catch (const std::exception& e) {
                logger_->error(std::string("WatchDog stop failed: ") + e.what());
            } catch (...) {
                logger_->error("WatchDog stop failed: unknown error");
            }
        }
        throw;
    }
}

void WatchDog::kick() {
    if (logger_) {
        logger_->info("WatchDog kick called");
    }
    try {
        timer_service_->stop();
        timer_service_->start(message_queue_, message_);
        if (logger_) {
            logger_->info("WatchDog kick succeeded");
        }
    } catch (...) {
        if (logger_) {
            try {
                throw;
            } catch (const std::exception& e) {
                logger_->error(std::string("WatchDog kick failed: ") + e.what());
            } catch (...) {
                logger_->error("WatchDog kick failed: unknown error");
            }
        }
        throw;
    }
}

} // namespace device_reminder

