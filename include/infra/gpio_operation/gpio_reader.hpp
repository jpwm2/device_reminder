#pragma once

#include "infra/gpio_operation/i_gpio_reader.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/file_loader/i_file_loader.hpp"

#include <memory>
#include <string>

namespace device_reminder {

class GPIOReader : public IGPIOReader {
public:
    GPIOReader(std::shared_ptr<ILogger> logger,
               int pin_no,
               std::shared_ptr<IFileLoader> chip_loader);

    bool read() override;
    void poll_edge(bool detect_rising) override;

private:
    std::shared_ptr<ILogger> logger_;
    int pin_no_;
    std::shared_ptr<IFileLoader> chip_loader_;
    std::string chip_name_;
};

} // namespace device_reminder

