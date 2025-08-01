#pragma once

#include "core/main_task/i_main_process.hpp"
#include "infra/process_operation/process_base/process_base.hpp"
#include "infra/watch_dog/i_watch_dog.hpp"
#include <memory>

namespace device_reminder {

class MainProcess : public ProcessBase, public IMainProcess {
public:
    MainProcess(std::shared_ptr<IProcessReceiver> receiver,
                std::shared_ptr<IProcessSender> sender,
                std::shared_ptr<IProcessQueue> queue,
                std::shared_ptr<IFileLoader> file_loader,
                std::shared_ptr<ILogger> logger,
                std::shared_ptr<IWatchDog> watchdog);

    int run() override;
    void stop() override;

private:
    std::shared_ptr<IWatchDog> watchdog_;
};

} // namespace device_reminder
