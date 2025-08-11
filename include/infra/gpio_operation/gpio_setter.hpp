#pragma once

// GPIOSetter インターフェースおよび実装

#include "infra/logger/i_logger.hpp"
#include "infra/file_loader/i_file_loader.hpp"

#include <memory>

struct gpiod_chip;
struct gpiod_line;

namespace device_reminder {

class IGPIOSetter {
public:
    virtual ~IGPIOSetter() = default;

    virtual void write(bool is_high) = 0;
    virtual void buzz_start(int frequency_hz) = 0;
    virtual void buzz_stop() = 0;
};

class GPIOSetter : public IGPIOSetter {
public:
    GPIOSetter(std::shared_ptr<ILogger>     logger,
               int                          pin_no,
               std::shared_ptr<IFileLoader> loader);
    ~GPIOSetter() override;

    void write(bool is_high) override;
    void buzz_start(int frequency_hz) override;
    void buzz_stop() override;

private:
    std::shared_ptr<ILogger>     logger_{};
    int                          pin_no_{};
    std::shared_ptr<IFileLoader> loader_{};
    gpiod_chip*                  chip_{};
    gpiod_line*                  line_{};
};

} // namespace device_reminder

