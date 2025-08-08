#include "infra/process_operation/process_sender/process_sender.hpp"

#include <utility>
#include <exception>
#include <string>

namespace device_reminder {

ProcessSender::ProcessSender(std::shared_ptr<IProcessQueue> queue,
                             std::shared_ptr<IMessage> message,
                             std::shared_ptr<ILogger> logger)
    : queue_{std::move(queue)}
    , message_{std::move(message)}
    , logger_{std::move(logger)} {}

void ProcessSender::send() {
    if (logger_) {
        logger_->info("メッセージ送信開始");
    }
    try {
        if (queue_) {
            queue_->push(message_);
        }
        if (logger_) {
            logger_->info("メッセージ送信成功");
        }
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error(std::string("メッセージ送信失敗: ") + e.what());
        }
        throw;
    }
}

} // namespace device_reminder
