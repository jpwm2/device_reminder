#include "main_task/main_task_handler.hpp"

namespace device_reminder {
void MainTaskHandler::handle(const Message& msg) {
    if (task_) task_->run(msg);
}

} // namespace device_reminder
