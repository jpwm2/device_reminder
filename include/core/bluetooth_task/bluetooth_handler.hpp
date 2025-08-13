#pragma once

#include "infra/logger/i_logger.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include "infra/bluetooth_driver/i_bluetooth_driver.hpp"
#include "infra/message/message_queue.hpp"
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
                     std::shared_ptr<IMessageQueue> main_queue,
                     std::shared_ptr<IMessage> success_msg,
                     std::shared_ptr<IMessage> failure_msg);

    std::vector<std::string> scan() override;

private:
    std::shared_ptr<ILogger> logger_{};
    std::shared_ptr<IFileLoader> loader_{};
    std::shared_ptr<IProcessSender> sender_{};
    std::shared_ptr<IBluetoothDriver> driver_{};
    std::shared_ptr<IMessageQueue> main_queue_{};
    std::shared_ptr<IMessage> success_msg_{};
    std::shared_ptr<IMessage> failure_msg_{};
};

} // namespace device_reminder

