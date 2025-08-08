#include "infra/thread_operation/thread_queue/thread_queue.hpp"
#include "infra/logger/i_logger.hpp"
#include <stdexcept>
#include <utility>

namespace device_reminder {

ThreadQueue::ThreadQueue(std::shared_ptr<ILogger> logger)
    : logger_(std::move(logger)) {}

void ThreadQueue::push(std::shared_ptr<IMessage> msg) {
  if (!msg) {
    if (logger_) {
      logger_->error("ThreadQueue: null メッセージ");
    }
    throw std::invalid_argument("msg is null");
  }
  {
    std::lock_guard<std::mutex> lock(mtx_);
    q_.push(std::move(msg));
  }
  if (logger_) {
    logger_->info("キューへメッセージ追加成功");
  }
}

std::shared_ptr<IMessage> ThreadQueue::pop() {
  std::shared_ptr<IMessage> msg;
  {
    std::lock_guard<std::mutex> lock(mtx_);
    if (q_.empty()) {
      return nullptr;
    }
    msg = q_.front();
    q_.pop();
  }
  if (logger_) {
    logger_->info("キューからメッセージ取得成功");
  }
  return msg;
}

size_t ThreadQueue::size() const {
  std::lock_guard<std::mutex> lock(mtx_);
  return q_.size();
}

} // namespace device_reminder
