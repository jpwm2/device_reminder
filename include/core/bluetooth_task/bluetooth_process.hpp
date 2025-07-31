#pragma once

#include "core/bluetooth_task/i_bluetooth_process.hpp"
#include "infra/process_operation/process_base/process_base.hpp"
#include "infra/watch_dog/i_watch_dog.hpp"
#include "core/interfaces/i_handler.hpp"
#include "core/bluetooth_task/i_bluetooth_task.hpp"

#include <memory>

namespace device_reminder {

class BluetoothProcess : public ProcessBase, public IBluetoothProcess {
public:
    BluetoothProcess(std::shared_ptr<IProcessQueue> queue,
                     std::shared_ptr<IProcessReceiver> receiver,
                     std::shared_ptr<IProcessDispatcher> dispatcher,
                     std::shared_ptr<IProcessSender> sender,
                     std::shared_ptr<IFileLoader> file_loader,
                     std::shared_ptr<ILogger> logger,
                     std::shared_ptr<IWatchDog> watch_dog,
                     std::shared_ptr<IHandler> handler,
                     std::shared_ptr<IBluetoothTask> task);

    void run() override;
    void stop() override;

private:
    std::shared_ptr<IBluetoothTask> bluetooth_task_;
    std::shared_ptr<IHandler>       handler_;
    std::shared_ptr<IWatchDog>      watch_dog_;
};

} // namespace device_reminder
