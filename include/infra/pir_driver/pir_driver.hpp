#pragma once

#include "pir_driver/i_pir_driver.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/gpio_operation/i_gpio_reader.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/thread_operation/thread_sender/i_thread_sender.hpp"
#include <atomic>
#include <memory>
#include <string>
#include <thread>

namespace device_reminder {

class PIRDriver : public IPIRDriver {
public:
    PIRDriver(std::shared_ptr<IFileLoader> gpiochip_loader,
              std::shared_ptr<ILogger> logger,
              std::shared_ptr<IThreadSender> sender,
              std::shared_ptr<IGPIOReader> reader,
              IFileLoader& pin_loader);

    ~PIRDriver() override;

    void run() override;
    void stop() override;

private:
    std::shared_ptr<IFileLoader> gpiochip_loader_;
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IThreadSender> sender_;
    std::shared_ptr<IGPIOReader> reader_;
    IFileLoader& pin_loader_;
    std::thread thread_;
    std::atomic<bool> running_{false};
    std::exception_ptr thread_exception_{};
};

} // namespace device_reminder
