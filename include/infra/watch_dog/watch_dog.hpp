#pragma once

#include "infra/watch_dog/i_watch_dog.hpp"
#include "infra/timer_service/i_timer_service.hpp"
#include "infra/logger/i_logger.hpp"
#include <memory>

namespace device_reminder {

class WatchDog : public IWatchDog {
public:
    explicit WatchDog(std::shared_ptr<ITimerService> timer_service,
                      std::shared_ptr<ILogger>     logger = nullptr);

    void start() override;
    void stop() override;
    void kick() override;

private:
    std::shared_ptr<ITimerService> timer_service_;
    std::shared_ptr<ILogger>       logger_;
    bool                           running_{false};
};

} // namespace device_reminder

