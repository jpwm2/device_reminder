#include "watch_dog/watch_dog.hpp"
#include "infra/timer_service/i_timer_service.hpp"

namespace device_reminder {

WatchDog::WatchDog(std::shared_ptr<ITimerService> timer_service,
                   std::shared_ptr<ILogger>     logger)
    : timer_service_(std::move(timer_service)),
      logger_(std::move(logger)) {}

void WatchDog::start() {
    if (!timer_service_ || running_) return;

    if (logger_) logger_->info("WDT監視開始");
    timer_service_->start();
    running_ = true;
}

void WatchDog::stop() {
    if (!timer_service_ || !running_) return;

    if (logger_) logger_->info("WDT監視停止");
    timer_service_->stop();
    running_ = false;
}

void WatchDog::kick() {
    if (!timer_service_) return;

    if (logger_) logger_->info("WDT kick");
    timer_service_->stop();
    timer_service_->start();
    running_ = true;
}

} // namespace device_reminder

