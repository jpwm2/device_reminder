#pragma once

#include "infra/file_loader/i_file_loader.hpp"
#include "infra/gpio_operation/gpio_reader.hpp"
#include "infra/logger/i_logger.hpp"

#include <atomic>
#include <memory>

namespace device_reminder {

class IPIRDriver {
public:
    virtual ~IPIRDriver() = default;

    virtual void run() = 0;
    virtual void stop() = 0;
};

class PIRDriver : public IPIRDriver {
public:
    PIRDriver(std::shared_ptr<IFileLoader> loader,
              std::shared_ptr<ILogger> logger,
              std::shared_ptr<IGPIOReader> gpio);

    void run() override;
    void stop() override;

private:
    std::shared_ptr<IFileLoader> loader_;
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IGPIOReader> gpio_;
    std::atomic<bool> monitoring_{false};
    std::atomic<bool> interrupt_{false};
};

} // namespace device_reminder
