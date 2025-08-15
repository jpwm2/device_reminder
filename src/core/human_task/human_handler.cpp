#include "human_task/human_handler.hpp"
#include "infra/message/thread_sender.hpp"

namespace device_reminder {

HumanHandler::HumanHandler(std::shared_ptr<ILogger> logger,
                           std::shared_ptr<IPIRDriver> pir,
                           std::shared_ptr<ITimerService> timer,
                           std::shared_ptr<IProcessSender> sender,
                           std::shared_ptr<IFileLoader> loader,
                           std::shared_ptr<IMessage> cooldown_msg,
                           std::string main_endpoint,
                           std::shared_ptr<IMessageQueue> main_queue,
                           std::shared_ptr<IMessage> success_msg)
    : logger_(std::move(logger))
    , pir_(std::move(pir))
    , timer_(std::move(timer))
    , sender_(std::move(sender))
    , loader_(std::move(loader))
    , cooldown_msg_(std::move(cooldown_msg))
    , main_endpoint_(std::move(main_endpoint))
    , main_queue_(std::move(main_queue))
    , success_msg_(std::move(success_msg)) {}

void HumanHandler::get_detect() {
    if (logger_) logger_->info("[HumanHandler::get_detect] start");
    try {
        if (sender_ && !main_endpoint_.empty() && success_msg_) {
            sender_->send(main_endpoint_, success_msg_);
        }

        if (timer_ && main_queue_ && cooldown_msg_) {
            auto thread_sender = std::make_shared<ThreadSender>(logger_);
            timer_->start(0, thread_sender, main_queue_, cooldown_msg_);
        }

        if (logger_) logger_->info("[HumanHandler::get_detect] success");
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("[HumanHandler::get_detect] failed: ") + e.what());
        throw;
    }
}

void HumanHandler::start_detect() {
    if (logger_) logger_->info("[HumanHandler::start_detect] start");
    try {
        if (pir_) {
            pir_->run();
        }
        if (logger_) logger_->info("[HumanHandler::start_detect] success");
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("[HumanHandler::start_detect] failed: ") + e.what());
        throw;
    }
}

} // namespace device_reminder

