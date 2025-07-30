#pragma once

#include "infra/process_operation/process_message/i_process_message.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include <memory>

namespace device_reminder {

class ProcessSender : public IProcessSender {
public:
  ProcessSender(std::shared_ptr<IProcessQueue> queue,
                std::shared_ptr<IProcessMessage> msg);

  void send() override;

private:
  std::shared_ptr<IProcessQueue> queue_;
  std::shared_ptr<IProcessMessage> msg_;
};

} // namespace device_reminder
