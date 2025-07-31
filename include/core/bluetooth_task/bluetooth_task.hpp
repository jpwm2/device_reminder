#pragma once

#include "bluetooth_task/i_bluetooth_task.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/bluetooth_driver/i_bluetooth_driver.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include <memory>

namespace device_reminder {

class BluetoothTask : public IBluetoothTask {
public:
    enum class State { WaitRequest, Scanning };

    BluetoothTask(std::shared_ptr<ILogger> logger,
                  std::shared_ptr<IProcessSender> sender,
                  std::shared_ptr<IFileLoader> loader,
                  std::shared_ptr<IBluetoothDriver> driver);
    ~BluetoothTask();

    void on_waiting(const std::vector<std::string>& payload) override;

    State state() const noexcept { return state_; }

private:
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IProcessSender> sender_;
    std::shared_ptr<IFileLoader> loader_;
    std::shared_ptr<IBluetoothDriver> driver_;
    State state_{State::WaitRequest};
};

} // namespace device_reminder
