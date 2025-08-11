#include "buzzer_driver/buzzer_driver.hpp"

#include <stdexcept>

namespace device_reminder {

namespace {
constexpr char kConfigFile[] = "/etc/device_reminder/buzzer.conf";
constexpr char kFrequencyKey[] = "frequency_hz";
} // namespace

BuzzerDriver::BuzzerDriver(std::shared_ptr<IFileLoader> loader,
                           std::shared_ptr<ILogger> logger,
                           std::shared_ptr<IGPIOSetter> gpio)
    : loader_(std::move(loader)),
      logger_(std::move(logger)),
      gpio_(std::move(gpio))
{}

void BuzzerDriver::on() {
    if (logger_) logger_->info("BuzzerDriver::on start");
    try {
        std::string freq_str = loader_->load_string(kConfigFile, kFrequencyKey);
        int frequency = 0;
        try {
            frequency = std::stoi(freq_str);
        } catch (const std::exception&) {
            throw std::invalid_argument("invalid frequency: " + freq_str);
        }
        gpio_->buzz_start(frequency);
        if (logger_)
            logger_->info("BuzzerDriver::on success: frequency=" +
                           std::to_string(frequency));
    } catch (const std::exception& e) {
        if (logger_)
            logger_->error(std::string("BuzzerDriver::on failed: ") + e.what());
        throw;
    }
}

void BuzzerDriver::off() {
    if (logger_) logger_->info("BuzzerDriver::off start");
    try {
        gpio_->buzz_stop();
        if (logger_) logger_->info("BuzzerDriver::off success");
    } catch (const std::exception& e) {
        if (logger_)
            logger_->error(std::string("BuzzerDriver::off failed: ") + e.what());
        throw;
    }
}

} // namespace device_reminder
