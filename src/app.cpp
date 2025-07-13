#include "app/app.hpp"

namespace device_reminder {

App::App(std::unique_ptr<IMainProcess> main_process,
         std::unique_ptr<IHumanProcess> human_process,
         std::unique_ptr<IBluetoothProcess> bluetooth_process,
         std::unique_ptr<IBuzzerProcess> buzzer_process,
         std::unique_ptr<ILogger> logger)
    : main_process_(std::move(main_process))
    , human_process_(std::move(human_process))
    , bluetooth_process_(std::move(bluetooth_process))
    , buzzer_process_(std::move(buzzer_process))
    , logger_(std::move(logger)) {}

int App::run() {
    try {
        main_process_->run();
        human_process_->run();
        bluetooth_process_->run();
        buzzer_process_->run();
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
