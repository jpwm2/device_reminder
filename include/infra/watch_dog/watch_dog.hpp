#pragma once

#include "infra/watch_dog/i_watch_dog.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/timer_service/i_timer_service.hpp"

#include <functional>
#include <memory>

namespace device_reminder {

class WatchDog : public IWatchDog {
public:
    WatchDog(std::shared_ptr<ILogger> logger,
             std::shared_ptr<ITimerService> timer,
             std::function<void()> on_timeout_handler,
             int timeout_ms);

    void start() override;
    void stop() override;
    void kick() override;

private:
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<ITimerService> timer_;
    std::function<void()> on_timeout_;
    int timeout_ms_{0};
};

} // namespace device_reminder

