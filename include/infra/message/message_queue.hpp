#pragma once
#include "infra/logger/i_logger.hpp"
#include "infra/thread_operation/thread_queue/i_thread_queue.hpp"
#include <mutex>
#include <queue>

namespace device_reminder {

class ThreadQueue : public IThreadQueue {
public:
  explicit ThreadQueue(std::shared_ptr<ILogger> logger);

  void push(std::shared_ptr<IThreadMessage> msg) override;
  std::shared_ptr<IThreadMessage> pop() override;
  size_t size() const override;

private:
  std::shared_ptr<ILogger> logger_;
  mutable std::mutex mtx_;
  std::queue<std::shared_ptr<IThreadMessage>> q_;
};

} // namespace device_reminder
