#include "buzzer_driver/buzzer_driver.hpp"
#include <filesystem>
#include <fstream>
#include <cmath>
#include <stdexcept>

namespace device_reminder {
namespace fs = std::filesystem;

BuzzerDriver::BuzzerDriver(IGPIODriver* gpio,
                           ILogger* logger,
                           std::string chip,
                           unsigned int line,
                           double freq_hz,
                           double duty,
                           std::string sysfs_base)
    : chipPath_(std::move(chip)),
      sysfsBase_(std::move(sysfs_base)),
      line_(line),
      freq_(freq_hz),
      duty_(duty),
      pwmChip_(0),
      pwmChannel_(0),
      gpio_(gpio),
      logger_(logger)
{
    try {
        if (logger_) logger_->info("BuzzerDriver created");
        exportPwm();
        setFrequency(freq_);
        setDutyCycle(duty_);
        enable(false);
    } catch (const std::exception& ex) {
        if (logger_) logger_->error(std::string("BuzzerDriver init failed: ") + ex.what());
        throw;
    }
}

BuzzerDriver::~BuzzerDriver() {
    stop();
    unexportPwm();
    if (logger_) logger_->info("BuzzerDriver destroyed");
}

bool BuzzerDriver::start() {
    if (isOn_) return true;
    if (!setFrequency(freq_)) return false;
    if (!setDutyCycle(duty_)) return false;
    if (!enable(true)) return false;
    isOn_ = true;
    return true;
}

bool BuzzerDriver::stop() {
    if (!isOn_) return true;
    if (!enable(false)) return false;
    isOn_ = false;
    return true;
}

bool BuzzerDriver::exportPwm() {
    fs::path path = fs::path(sysfsBase_) / ("pwmchip" + std::to_string(pwmChip_)) / "export";
    std::ofstream ofs(path);
    if (!ofs) {
        if (logger_) logger_->error("failed to open " + path.string());
        return false;
    }
    ofs << pwmChannel_;
    return ofs.good();
}

bool BuzzerDriver::unexportPwm() {
    fs::path path = fs::path(sysfsBase_) / ("pwmchip" + std::to_string(pwmChip_)) / "unexport";
    std::ofstream ofs(path);
    if (!ofs) {
        if (logger_) logger_->error("failed to open " + path.string());
        return false;
    }
    ofs << pwmChannel_;
    return ofs.good();
}

bool BuzzerDriver::setFrequency(double hz) {
    if (hz <= 0) return false;
    periodNs_ = static_cast<unsigned int>(std::round(1e9 / hz));
    fs::path path = fs::path(sysfsBase_) / ("pwmchip" + std::to_string(pwmChip_)) /
                    ("pwm" + std::to_string(pwmChannel_)) / "period";
    std::ofstream ofs(path);
    if (!ofs) {
        if (logger_) logger_->error("failed to open " + path.string());
        return false;
    }
    ofs << periodNs_;
    return ofs.good();
}

bool BuzzerDriver::setDutyCycle(double ratio) {
    unsigned int duty_ns = static_cast<unsigned int>(periodNs_ * ratio);
    fs::path path = fs::path(sysfsBase_) / ("pwmchip" + std::to_string(pwmChip_)) /
                    ("pwm" + std::to_string(pwmChannel_)) / "duty_cycle";
    std::ofstream ofs(path);
    if (!ofs) {
        if (logger_) logger_->error("failed to open " + path.string());
        return false;
    }
    ofs << duty_ns;
    return ofs.good();
}

bool BuzzerDriver::enable(bool on) {
    fs::path path = fs::path(sysfsBase_) / ("pwmchip" + std::to_string(pwmChip_)) /
                    ("pwm" + std::to_string(pwmChannel_)) / "enable";
    std::ofstream ofs(path);
    if (!ofs) {
        if (logger_) logger_->error("failed to open " + path.string());
        return false;
    }
    ofs << (on ? 1 : 0);
    return ofs.good();
}

} // namespace device_reminder
