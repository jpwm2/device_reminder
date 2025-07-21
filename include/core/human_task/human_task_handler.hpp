#pragma once

#include "interfaces/i_message_handler.hpp"
#include "infra/thread_message_operation/thread_message.hpp"
#include "human_task/human_task.hpp"
#include <memory>

namespace device_reminder {

class HumanTaskHandler : public IMessageHandler {
public:
    explicit HumanTaskHandler(std::shared_ptr<HumanTask> task)
        : task_(std::move(task)) {}

    void handle(const ThreadMessage& msg) override;

private:
    std::shared_ptr<HumanTask> task_;
};

} // namespace device_reminder
