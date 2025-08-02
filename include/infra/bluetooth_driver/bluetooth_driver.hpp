#pragma once

#include "i_bluetooth_driver.hpp"
#include "i_bluetooth_scanner.hpp"
#include "i_bluetooth_pairer.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/thread_operation/thread_sender/i_thread_sender.hpp"
#include "infra/logger/i_logger.hpp"

#include <vector>
#include <string>
#include <memory>

namespace device_reminder {

class BluetoothDriver : public IBluetoothDriver {
public:
    BluetoothDriver(std::shared_ptr<IFileLoader> device_loader,
                    std::shared_ptr<IFileLoader> distance_threshold_loader,
                    std::shared_ptr<IBluetoothScanner> scanner,
                    std::shared_ptr<IBluetoothPairer> pairer,
                    std::shared_ptr<IThreadSender> sender,
                    std::shared_ptr<ILogger> logger);

    void run() override;
    void stop() override;
    std::vector<std::string> scan() override;

    const std::vector<std::string>& paired_devices() const { return paired_devices_; }

private:
    std::shared_ptr<IFileLoader> device_loader_;
    std::shared_ptr<IFileLoader> distance_threshold_loader_;
    std::shared_ptr<IBluetoothScanner> scanner_;
    std::shared_ptr<IBluetoothPairer> pairer_;
    std::shared_ptr<IThreadSender> sender_;
    std::shared_ptr<ILogger> logger_;
    std::vector<std::string> paired_devices_;
    bool running_{false};
};

} // namespace device_reminder
