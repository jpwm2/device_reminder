#include "infra/thread_operation/thread_sender/thread_sender.hpp"
#include "infra/logger/i_logger.hpp"

#include <exception>
#include <utility>

namespace device_reminder {

ThreadSender::ThreadSender(std::shared_ptr<ILogger> logger,
                           std::shared_ptr<IThreadQueue> queue,
                           std::shared_ptr<IMessage> message)
    : logger_(std::move(logger)), queue_(std::move(queue)),
      message_(std::move(message)) {}

void ThreadSender::send() {
  try {
    queue_->push(message_);
    if (logger_) {
      logger_->info("ThreadSender: メッセージ送信に成功しました");
    }
  } catch (const std::exception &e) {
    if (logger_) {
      logger_->error(
          std::string("ThreadSender: メッセージ送信に失敗しました: ") +
          e.what());
    }
    throw;
  }
}

} // namespace device_reminder
