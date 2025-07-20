// buzzer_driver_impl.hpp
#pragma once
#include "i_buzzer_driver.hpp"
#include "infra/logger/i_logger.hpp"
#include <iostream>
#include <memory>

namespace device_reminder {

class BuzzerDriver : public IBuzzerDriver {
public:
    explicit BuzzerDriver(std::shared_ptr<ILogger> logger)
        : logger_(std::move(logger)) {}

    void start_buzzer() override {
        if (logger_) logger_->info("Start Buzzer");
        std::cout << "Start Buzzer\n";
    }
    void stop_buzzer() override {
        if (logger_) logger_->info("Stop Buzzer");
        std::cout << "Stop Buzzer\n";
    }

private:
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder
