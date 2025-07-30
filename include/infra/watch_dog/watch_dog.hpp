#pragma once

#include "infra/watch_dog/i_watch_dog.hpp"
#include "infra/timer_service/i_timer_service.hpp"
#include <memory>

namespace device_reminder {

class WatchDog : public IWatchDog {
public:
    explicit WatchDog(std::shared_ptr<ITimerService> timer_service);

    void start() override;
    void stop() override;
    void kick() override;

private:
    std::shared_ptr<ITimerService> timer_service_;
    bool running_{false};
};

} // namespace device_reminder

