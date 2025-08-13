#pragma once

#include "infra/logger.hpp"
#include "infra/pir_driver/pir_driver.hpp"
#include "infra/timer_service/timer_service.hpp"
#include "infra/file_loader.hpp"
#include "infra/message/process_sender.hpp"
#include "infra/message/message.hpp"
#include "infra/message/message_queue.hpp"

#include <memory>

namespace device_reminder {

class IHumanHandler {
public:
    virtual ~IHumanHandler() = default;
    virtual void get_detect() = 0;
    virtual void start_detect() = 0;
};

class HumanHandler : public IHumanHandler {
public:
    HumanHandler(std::shared_ptr<ILogger> logger,
                 std::shared_ptr<IPIRDriver> pir,
                 std::shared_ptr<ITimerService> timer,
                 std::shared_ptr<IProcessSender> sender,
                 std::shared_ptr<IFileLoader> loader,
                 std::shared_ptr<IMessage> cooldown_msg,
                 std::shared_ptr<IMessageQueue> main_queue,
                 std::shared_ptr<IMessage> success_msg);

    void get_detect() override;
    void start_detect() override;

private:
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IPIRDriver> pir_;
    std::shared_ptr<ITimerService> timer_;
    std::shared_ptr<IProcessSender> sender_;
    std::shared_ptr<IFileLoader> loader_;
    std::shared_ptr<IMessage> cooldown_msg_;
    std::shared_ptr<IMessageQueue> main_queue_;
    std::shared_ptr<IMessage> success_msg_;
};

} // namespace device_reminder

