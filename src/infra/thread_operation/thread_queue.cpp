#include "infra/thread_operation/thread_queue/thread_queue.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/thread_operation/thread_message/i_thread_message.hpp"
#include <utility>

namespace device_reminder {

ThreadQueue::ThreadQueue(std::shared_ptr<ILogger> logger)
    : logger_(std::move(logger)) {}

void ThreadQueue::push(std::shared_ptr<IThreadMessage> msg) {
  {
    std::lock_guard<std::mutex> lock(mtx_);
    q_.push(std::move(msg));
  }
  if (logger_) {
    logger_->info("ThreadQueue push");
  }
}

std::shared_ptr<IThreadMessage> ThreadQueue::pop() {
  std::lock_guard<std::mutex> lock(mtx_);
  if (q_.empty())
    return nullptr;
  auto msg = q_.front();
  q_.pop();
  return msg;
}

size_t ThreadQueue::size() const {
  std::lock_guard<std::mutex> lock(mtx_);
  return q_.size();
}

} // namespace device_reminder
