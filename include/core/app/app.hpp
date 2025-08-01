#pragma once
#include <memory>

#include "main_task/i_main_process.hpp"
#include "human_task/i_human_process.hpp"
#include "bluetooth_task/i_bluetooth_process.hpp"
#include "buzzer_task/i_buzzer_process.hpp"
#include "infra/logger/i_logger.hpp"

namespace device_reminder {


class App {
public:
    App(std::unique_ptr<IMainProcess> main_process,
        std::unique_ptr<IHumanProcess> human_process,
        std::unique_ptr<IBluetoothProcess> bluetooth_process,
        std::unique_ptr<IBuzzerProcess> buzzer_process,
        std::unique_ptr<ILogger> logger);

    int run();

private:
    std::unique_ptr<IMainProcess> main_process_;
    std::unique_ptr<IHumanProcess> human_process_;
    std::unique_ptr<IBluetoothProcess> bluetooth_process_;
    std::unique_ptr<IBuzzerProcess> buzzer_process_;
    std::unique_ptr<ILogger> logger_;
};

} // namespace device_reminder
