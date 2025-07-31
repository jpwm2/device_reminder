#pragma once

#include "pir_driver/i_pir_driver.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/gpio_operation/gpio_reader/i_gpio_reader.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/thread_operation/thread_sender/i_thread_sender.hpp"

#include <memory>
#include <string>

namespace device_reminder {

class PIRDriver : public IPIRDriver {
public:
    PIRDriver(std::shared_ptr<IFileLoader> loader,
              std::shared_ptr<ILogger> logger,
              std::shared_ptr<IThreadSender> sender,
              std::shared_ptr<IGPIOReader> gpio);

    ~PIRDriver() override;

    void run() override;
    void stop() override;

private:
    std::shared_ptr<IFileLoader> loader_;
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IThreadSender> sender_;
    std::shared_ptr<IGPIOReader> gpio_;
    std::thread thread_;
    std::atomic<bool> running_{false};
    bool last_state_{false};
};

} // namespace device_reminder
