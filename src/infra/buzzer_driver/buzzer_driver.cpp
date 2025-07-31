#include "buzzer_driver/buzzer_driver.hpp"

namespace device_reminder {

BuzzerDriver::BuzzerDriver(std::shared_ptr<IFileLoader> loader,
                           std::shared_ptr<ILogger> logger,
                           std::shared_ptr<IGPIOSetter> gpio)
    : loader_(std::move(loader)),
      logger_(std::move(logger)),
      gpio_(std::move(gpio))
{
    if (logger_) logger_->info("BuzzerDriver created");
    if (loader_) {
        try {
            loader_->load_int(); // 設定値読み込み (エラー確認のみ)
        } catch (...) {
            if (logger_) logger_->error("Failed to load buzzer config");
        }
    }
}

void BuzzerDriver::on() {
    if (gpio_) {
        gpio_->write(true);
    }
    if (logger_) logger_->info("buzzer on");
}

void BuzzerDriver::off() {
    if (gpio_) {
        gpio_->write(false);
    }
    if (logger_) logger_->info("buzzer off");
}

} // namespace device_reminder
