#include "main_task/main_handler.hpp"
#include "infra/process_operation/process_message/process_message_type.hpp"
#include "infra/process_operation/process_message/i_process_message.hpp"

namespace device_reminder {

MainHandler::MainHandler(std::shared_ptr<ILogger> logger,
                         std::shared_ptr<IMainTask> task,
                         std::shared_ptr<ITimerService> timer_service,
                         std::shared_ptr<IFileLoader> file_loader)
    : logger_(std::move(logger))
    , task_(std::move(task))
    , timer_service_(std::move(timer_service))
    , file_loader_(std::move(file_loader)) {
    if (logger_) logger_->info("MainHandler created");
}

void MainHandler::handle(std::shared_ptr<IProcessMessage> msg) {
    if (!msg || !task_) return;

    switch (msg->type()) {
    case ProcessMessageType::HumanDetected:
        task_->on_waiting_for_human(msg->payload());
        break;
    case ProcessMessageType::ResponseDevicePresence:
        if (!msg->payload().empty() && msg->payload()[0] == "found") {
            task_->on_response_to_human_task(msg->payload());
        } else {
            task_->on_response_to_buzzer_task(msg->payload());
        }
        break;
    case ProcessMessageType::CoolDownTimeout:
        task_->on_cooldown(msg->payload());
        break;
    case ProcessMessageType::ScanTimeout:
        task_->on_waiting_for_second_response(msg->payload());
        break;
    default:
        break;
    }
}

} // namespace device_reminder
