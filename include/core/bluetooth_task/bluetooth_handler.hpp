#pragma once

#include "infra/logger.hpp"
#include "infra/file_loader.hpp"
#include "infra/message/process_sender.hpp"
#include "infra/bluetooth_driver/bluetooth_driver.hpp"
#include "infra/message/message.hpp"

#include <memory>
#include <string>
#include <vector>

namespace device_reminder {

class IBluetoothHandler {
public:
    virtual ~IBluetoothHandler() = default;
    virtual std::vector<std::string> scan() = 0;
};

class BluetoothHandler : public IBluetoothHandler {
public:
    BluetoothHandler(std::shared_ptr<ILogger> logger,
                     std::shared_ptr<IFileLoader> loader,
                     std::shared_ptr<IProcessSender> sender,
                     std::shared_ptr<IBluetoothDriver> driver,
                     std::string main_endpoint,
                     std::shared_ptr<IMessage> success_msg,
                     std::shared_ptr<IMessage> failure_msg);

    std::vector<std::string> scan() override;

private:
    std::shared_ptr<ILogger> logger_{};
    std::shared_ptr<IFileLoader> loader_{};
    std::shared_ptr<IProcessSender> sender_{};
    std::shared_ptr<IBluetoothDriver> driver_{};
    std::string main_endpoint_{};
    std::shared_ptr<IMessage> success_msg_{};
    std::shared_ptr<IMessage> failure_msg_{};
};

} // namespace device_reminder

