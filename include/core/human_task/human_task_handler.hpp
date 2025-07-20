#pragma once

#include "interfaces/i_message_handler.hpp"
#include "infra/message/message.hpp"
#include "human_task/human_task.hpp"
#include <memory>

namespace device_reminder {

class HumanTaskHandler : public IMessageHandler {
public:
    explicit HumanTaskHandler(std::shared_ptr<HumanTask> task)
        : task_(std::move(task)) {}

    void handle(const Message& msg) override;

private:
    std::shared_ptr<HumanTask> task_;
};

} // namespace device_reminder
