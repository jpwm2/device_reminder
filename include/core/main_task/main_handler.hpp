#pragma once

#include "infra/logger/i_logger.hpp"
#include "infra/message/message.hpp"
#include "infra/message/message_queue.hpp"
#include "infra/message/process_sender.hpp"
#include "infra/file_loader.hpp"
#include "infra/timer_service/timer_service.hpp"

#include <memory>

namespace device_reminder {

class IMainHandler {
public:
    virtual ~IMainHandler() = default;
    virtual void start_device_detect() = 0;
    virtual void end_device_first_detect() = 0;
    virtual void end_device_retry_detect() = 0;
    virtual void retry_device_detect() = 0;
    virtual void send_bt_request() = 0;
};

class MainHandler : public IMainHandler {
public:
    MainHandler(std::shared_ptr<ILogger> logger,
                std::shared_ptr<IProcessSender> sender,
                std::shared_ptr<IFileLoader> file_loader,
                std::shared_ptr<ITimerService> timer_service,
                std::shared_ptr<IMessageQueue> human_queue,
                std::shared_ptr<IMessage> human_start_msg,
                std::shared_ptr<IMessage> human_stop_msg,
                std::shared_ptr<IMessageQueue> bt_queue,
                std::shared_ptr<IMessage> device_scan_msg,
                std::shared_ptr<IMessageQueue> buzzer_queue,
                std::shared_ptr<IMessage> buzzer_start_msg,
                std::shared_ptr<IMessage> buzzer_stop_msg,
                std::shared_ptr<IMessageQueue> main_queue,
                std::shared_ptr<IMessage> bt_cooldown_end_msg,
                std::shared_ptr<IMessage> device_detect_timeout_msg);

    void start_device_detect() override;
    void end_device_first_detect() override;
    void end_device_retry_detect() override;
    void retry_device_detect() override;
    void send_bt_request() override;

private:
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IProcessSender> sender_;
    std::shared_ptr<IFileLoader> file_loader_;
    std::shared_ptr<ITimerService> timer_service_;
    std::shared_ptr<IMessageQueue> human_queue_;
    std::shared_ptr<IMessage> human_start_msg_;
    std::shared_ptr<IMessage> human_stop_msg_;
    std::shared_ptr<IMessageQueue> bt_queue_;
    std::shared_ptr<IMessage> device_scan_msg_;
    std::shared_ptr<IMessageQueue> buzzer_queue_;
    std::shared_ptr<IMessage> buzzer_start_msg_;
    std::shared_ptr<IMessage> buzzer_stop_msg_;
    std::shared_ptr<IMessageQueue> main_queue_;
    std::shared_ptr<IMessage> bt_cooldown_end_msg_;
    std::shared_ptr<IMessage> device_detect_timeout_msg_;
};

} // namespace device_reminder

