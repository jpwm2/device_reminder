#include "infra/process_operation/process_sender.hpp"

#include "infra/message/i_message.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/logger/i_logger.hpp"

#include <stdexcept>
#include <utility>

namespace device_reminder {

ProcessSender::ProcessSender(std::shared_ptr<IProcessQueue> queue,
                             std::shared_ptr<IMessage> msg,
                             std::shared_ptr<ILogger> logger)
    : queue_{std::move(queue)},
      msg_{std::move(msg)},
      logger_{std::move(logger)} {}

void ProcessSender::send() {
  try {
    if (!logger_) {
      throw std::runtime_error("logger is null");
    }

    logger_->info("メッセージ送信処理開始");

    if (!queue_) {
      throw std::runtime_error("queue is null");
    }

    queue_->enqueue(msg_);
    logger_->info("メッセージ送信成功");
  } catch (...) {
    if (logger_) {
      logger_->error("メッセージ送信失敗");
    }
    throw;
  }
}

} // namespace device_reminder

