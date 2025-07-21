#include "buzzer_task/buzzer_handler.hpp"

namespace device_reminder {

void BuzzerHandler::handle(const ThreadMessage& msg) {
    if (task_) task_->send_message(msg);
}

} // namespace device_reminder
