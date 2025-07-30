#include "infra/process_operation/process_sender/process_sender.hpp"

#include "infra/process_operation/process_message/i_process_message.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"

#include <utility>

namespace device_reminder {

ProcessSender::ProcessSender(std::shared_ptr<IProcessQueue> queue,
                             std::shared_ptr<IProcessMessage> msg)
    : queue_{std::move(queue)}, msg_{std::move(msg)} {}

void ProcessSender::send() {
  if (queue_) {
    queue_->push(msg_);
  }
}

} // namespace device_reminder
