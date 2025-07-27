#include "human_task/human_task_handler.hpp"

namespace device_reminder {

void HumanTaskHandler::handle(const ThreadMessage& msg) {
    if (task_) task_->run(msg);
}

} // namespace device_reminder

