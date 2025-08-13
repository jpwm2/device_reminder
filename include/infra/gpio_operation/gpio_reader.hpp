#pragma once

#include "infra/logger.hpp"
#include "infra/file_loader.hpp"

#include <memory>

struct gpiod_chip;
struct gpiod_line;

namespace device_reminder {

class IGPIOReader {
public:
    virtual ~IGPIOReader() = default;

    virtual bool read() = 0;
    virtual void poll_edge(bool detect_rising) = 0;
};

class GPIOReader : public IGPIOReader {
public:
    GPIOReader(std::shared_ptr<ILogger> logger,
               int pin_no,
               std::shared_ptr<IFileLoader> loader);
    ~GPIOReader() override;

    bool read() override;
    void poll_edge(bool detect_rising) override;

private:
    std::shared_ptr<ILogger>     logger_{};
    int                          pin_no_{};
    std::shared_ptr<IFileLoader> loader_{};
    gpiod_chip*                  chip_{};
    gpiod_line*                  line_{};
};

} // namespace device_reminder

