#pragma once

#include "infra/i_message.hpp"
#include "buzzer_task/i_buzzer_task.hpp"
#include "infra/buzzer_driver/i_buzzer_driver.hpp"
#include "infra/timer_service/i_timer_service.hpp"
#include "infra/logger/i_logger.hpp"

#include <memory>
#include <cstdint>

namespace device_reminder {

class BuzzerTask : public IBuzzerTask {
public:
    enum class State { WaitStart, Buzzing };

    BuzzerTask(std::shared_ptr<IBuzzerDriver> driver,
               std::shared_ptr<ITimerService> timer,
               std::shared_ptr<ILogger> logger = nullptr);

    void run() override {}
    bool send_message(const IMessage& msg) override;

    void onMessage(const IMessage& msg);
    State state() const noexcept { return state_; }

private:
    void startBuzzer();
    void stopBuzzer(bool cancelTimer);

    static constexpr uint32_t kBuzzDurationMs = 4000;

    std::shared_ptr<IBuzzerDriver>  driver_;
    std::shared_ptr<ITimerService>  timer_;
    std::shared_ptr<ILogger>        logger_;
    State                           state_{State::WaitStart};
};

} // namespace device_reminder
