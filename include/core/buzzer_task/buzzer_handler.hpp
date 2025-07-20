#pragma once

#include "interfaces/i_message_handler.hpp"
#include "buzzer_task/buzzer_task.hpp"
#include <memory>

namespace device_reminder {

class BuzzerHandler : public IMessageHandler {
public:
    explicit BuzzerHandler(std::shared_ptr<BuzzerTask> task)
        : task_(std::move(task)) {}

    void handle(const std::string& msg_str) override;

private:
    std::shared_ptr<BuzzerTask> task_;
};

} // namespace device_reminder
