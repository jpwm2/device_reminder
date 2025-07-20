#pragma once

#include "interfaces/i_message_handler.hpp"
#include "infra/message/message.hpp"
#include "buzzer_task/buzzer_task.hpp"
#include <memory>

namespace device_reminder {

class BuzzerHandler : public IMessageHandler {
public:
    explicit BuzzerHandler(std::shared_ptr<BuzzerTask> task)
        : task_(std::move(task)) {}

    void handle(const Message& msg) override;

private:
    std::shared_ptr<BuzzerTask> task_;
};

} // namespace device_reminder
