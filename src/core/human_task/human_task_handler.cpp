#include "human_task/human_task_handler.hpp"

namespace device_reminder {

void HumanTaskHandler::handle(const ThreadMessage& msg) {
    if (!task_) return;

    switch (msg.type()) {
    case ThreadMessageType::StartHumanDetection:
        state_ = State::Detecting;
        break;
    case ThreadMessageType::StopHumanDetection:
        state_ = State::Stopped;
        break;
    case ThreadMessageType::CooldownTimeout:
        state_ = State::Cooldown;
        break;
    default:
        break;
    }

    switch (state_) {
    case State::Detecting:
        task_->on_detecting(msg.payload());
        break;
    case State::Stopped:
        task_->on_stopping(msg.payload());
        break;
    case State::Cooldown:
        task_->on_cooldown(msg.payload());
        break;
    }
}

} // namespace device_reminder

