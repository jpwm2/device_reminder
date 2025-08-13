#pragma once
#include "infra/process/process.hpp"
#include "infra/logger.hpp"

namespace device_reminder {


class App {
public:
    App(IProcess& main_process,
        IProcess& human_process,
        IProcess& bluetooth_process,
        IProcess& buzzer_process,
        ILogger& logger);

    void run();

private:
    IProcess& main_process_;
    IProcess& human_process_;
    IProcess& bluetooth_process_;
    IProcess& buzzer_process_;
    ILogger& logger_;
};

} // namespace device_reminder
