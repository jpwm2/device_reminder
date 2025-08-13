#pragma once

#include "infra/logger.hpp"
#include "infra/file_loader.hpp"
#include "infra/buzzer_driver/buzzer_driver.hpp"
#include "infra/timer_service/timer_service.hpp"
#include "infra/message/message_queue.hpp"
#include "infra/message/message.hpp"

#include <memory>

namespace device_reminder {

class IBuzzerHandler {
public:
    virtual ~IBuzzerHandler() = default;
    virtual void start_buzzing_and_start_timer() = 0;
    virtual void stop_buzzing_and_stop_timer() = 0;
    virtual void stop_buzzing() = 0;
};

class BuzzerHandler : public IBuzzerHandler {
public:
    BuzzerHandler(std::shared_ptr<ILogger> logger,
                  std::shared_ptr<IFileLoader> file_loader,
                  std::shared_ptr<IBuzzerDriver> driver,
                  std::shared_ptr<ITimerService> timer_service,
                  std::shared_ptr<IMessageQueue> buzzer_queue,
                  std::shared_ptr<IMessage> buzzing_end_msg);

    void start_buzzing_and_start_timer() override;
    void stop_buzzing_and_stop_timer() override;
    void stop_buzzing() override;

private:
    std::shared_ptr<ILogger>       logger_{};
    std::shared_ptr<IFileLoader>   file_loader_{};
    std::shared_ptr<IBuzzerDriver> driver_{};
    std::shared_ptr<ITimerService> timer_service_{};
    std::shared_ptr<IMessageQueue> buzzer_queue_{};
    std::shared_ptr<IMessage>      buzzing_end_msg_{};
};

} // namespace device_reminder

