#pragma once

#include "interfaces/i_message_handler.hpp"
#include "infra/thread_operation/thread_message/thread_message.hpp"
#include "human_task/i_human_task.hpp"
#include <memory>

namespace device_reminder {

class HumanTaskHandler : public IMessageHandler {
public:
    enum class State { Detecting, Stopped, Cooldown };

    explicit HumanTaskHandler(std::shared_ptr<IHumanTask> task)
        : task_(std::move(task)) {}

    void handle(const ThreadMessage& msg) override;

private:
    std::shared_ptr<IHumanTask> task_;
    State state_{State::Detecting};
};

} // namespace device_reminder
