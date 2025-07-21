#include "app/app.hpp"
#include "thread_message_operation/thread_message.hpp"

namespace device_reminder {

App::App(std::unique_ptr<IMainTask> main_task,
         std::unique_ptr<IHumanTask> human_task,
         std::unique_ptr<IBluetoothTask> bluetooth_task,
         std::unique_ptr<IBuzzerTask> buzzer_task,
         std::unique_ptr<ILogger> logger)
    : main_task_(std::move(main_task))
    , human_task_(std::move(human_task))
    , bluetooth_task_(std::move(bluetooth_task))
    , buzzer_task_(std::move(buzzer_task))
    , logger_(std::move(logger)) {}

int App::run() {
    try {
        main_task_->run(ThreadMessage{});
        human_task_->run(ThreadMessage{});
        bluetooth_task_->run(ThreadMessage{});
        buzzer_task_->run();
    } catch (const std::exception& e) {
        logger_->error(std::string("[App::run] std::exception caught: ") + e.what());
        return 1;
    } catch (...) {
        logger_->error("[App::run] Unknown exception caught.");
        return 2;
    }
    logger_->info("[App::run] Completed successfully.");
    return 0;
}

} // namespace device_reminder
