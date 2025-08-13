#pragma once
#include "infra/process/process.hpp"
#include "infra/logger.hpp"
#include <memory>

namespace device_reminder {


class App {
public:
    App(std::unique_ptr<IProcess> main_process,
        std::unique_ptr<IProcess> human_process,
        std::unique_ptr<IProcess> bluetooth_process,
        std::unique_ptr<IProcess> buzzer_process,
        std::unique_ptr<ILogger> logger);

    void run();

private:
    std::unique_ptr<IProcess> main_process_;
    std::unique_ptr<IProcess> human_process_;
    std::unique_ptr<IProcess> bluetooth_process_;
    std::unique_ptr<IProcess> buzzer_process_;
    std::unique_ptr<ILogger> logger_;
};

} // namespace device_reminder
