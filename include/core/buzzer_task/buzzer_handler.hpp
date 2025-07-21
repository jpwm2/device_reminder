#pragma once

#include "interfaces/i_message_handler.hpp"
#include "infra/thread_message_operation/thread_message.hpp"
#include "buzzer_task/buzzer_task.hpp"
#include <memory>

namespace device_reminder {

class BuzzerHandler : public IMessageHandler {
public:
    explicit BuzzerHandler(std::shared_ptr<BuzzerTask> task)
        : task_(std::move(task)) {}

    void handle(const ThreadMessage& msg) override;

private:
    std::shared_ptr<BuzzerTask> task_;
};

} // namespace device_reminder
