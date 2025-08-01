#pragma once
#include <memory>

#include "main_task/i_main_task.hpp"
#include "human_task/i_human_task.hpp"
#include "bluetooth_task/i_bluetooth_task.hpp"
#include "buzzer_task/i_buzzer_task.hpp"
#include "infra/logger/i_logger.hpp"

namespace device_reminder {


class App {
public:
    App(std::unique_ptr<IMainTask> main_task,
        std::unique_ptr<IHumanTask> human_task,
        std::unique_ptr<IBluetoothTask> bluetooth_task,
        std::unique_ptr<IBuzzerTask> buzzer_task,
        std::unique_ptr<ILogger> logger);

    int run();

private:
    std::unique_ptr<IMainTask> main_task_;
    std::unique_ptr<IHumanTask> human_task_;
    std::unique_ptr<IBluetoothTask> bluetooth_task_;
    std::unique_ptr<IBuzzerTask> buzzer_task_;
    std::unique_ptr<ILogger> logger_;
};

} // namespace device_reminder
