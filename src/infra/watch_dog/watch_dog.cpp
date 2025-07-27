#include "watch_dog/watch_dog.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/process_message_operation/process_message.hpp"

namespace device_reminder {

WatchDog::WatchDog(std::shared_ptr<ILogger> logger,
                   std::shared_ptr<ITimerService> timer,
                   std::function<void()> on_timeout_handler,
                   int timeout_ms)
    : logger_(std::move(logger))
    , timer_(std::move(timer))
    , on_timeout_(std::move(on_timeout_handler))
    , timeout_ms_(timeout_ms) {
    if (logger_) logger_->info("WatchDog created");
}

void WatchDog::start() {
    if (!timer_) return;
    kick();
    if (logger_) logger_->info("WatchDog started");
}

void WatchDog::stop() {
    if (timer_) timer_->stop();
    if (logger_) logger_->info("WatchDog stopped");
}

void WatchDog::kick() {
    if (!timer_) return;
    ProcessMessage msg{ThreadMessageType::Timeout};
    timer_->start(static_cast<uint32_t>(timeout_ms_), msg);
    if (logger_) logger_->info("WatchDog kicked");
}

} // namespace device_reminder

