#pragma once

#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/message/i_message.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include <memory>

namespace device_reminder {

class ProcessSender : public IProcessSender {
public:
    ProcessSender(std::shared_ptr<IProcessQueue> queue,
                  std::shared_ptr<IMessage> message,
                  std::shared_ptr<ILogger> logger);

    void send() override;

private:
    std::shared_ptr<IProcessQueue> queue_;
    std::shared_ptr<IMessage> message_;
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder
