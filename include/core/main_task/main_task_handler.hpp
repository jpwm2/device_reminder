#pragma once

#include "interfaces/i_message_handler.hpp"
#include "main_task/main_task.hpp"
#include <memory>

namespace device_reminder {

class MainTaskHandler : public IMessageHandler {
public:
    explicit MainTaskHandler(std::shared_ptr<MainTask> task)
        : task_(std::move(task)) {}

    void handle(const std::string& msg_str) override;

private:
    std::shared_ptr<MainTask> task_;
};

} // namespace device_reminder
