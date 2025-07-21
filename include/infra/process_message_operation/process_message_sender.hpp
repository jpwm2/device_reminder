#pragma once
#include "infra/process_message_operation/process_message_queue.hpp"
#include "infra/process_message_operation/i_process_message_sender.hpp"
#include "infra/logger/i_logger.hpp"
#include <memory>
#include <string>

namespace device_reminder {

class ProcessMessageSender : public IProcessMessageSender {
public:
    explicit ProcessMessageSender(const std::string& queue_name,
                                  long max_messages = 10,
                                  std::shared_ptr<ILogger> logger = nullptr);
    ~ProcessMessageSender() override;

    bool enqueue(const ProcessMessage& msg) override;
    void stop() override;

private:
    std::string     queue_name_;
    ProcessMessageQueue mq_;
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder
