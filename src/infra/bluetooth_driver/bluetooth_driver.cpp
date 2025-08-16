#include "bluetooth_driver/bluetooth_driver.hpp"

#include "bluetooth_driver/bluetooth_scanner.hpp"
#include "bluetooth_driver/bluetooth_pairer.hpp"
#include "infra/logger.hpp"

#include <string>
#include <utility>
#include <vector>

namespace device_reminder {

BluetoothDriver::BluetoothDriver(std::shared_ptr<IBluetoothScanner> scanner,
                                 std::shared_ptr<IBluetoothPairer> pairer,
                                 std::shared_ptr<ILogger> logger)
    : scanner_(std::move(scanner))
    , pairer_(std::move(pairer))
    , logger_(std::move(logger)) {}

void BluetoothDriver::run() {
    if (logger_) {
        logger_->info("BluetoothDriver run start");
    }
    try {
        running_ = true;
        while (running_) {
            auto devices = scanner_ ? scanner_->scan() : std::vector<BluetoothDevice>{};
            bool paired = false;
            for (const auto& dev : devices) {
                if (dev.rssi >= -50) {
                    if (pairer_ && pairer_->pair(dev.mac)) {
                        paired    = true;
                        running_ = false;
                        break;
                    }
                }
            }
            if (paired) {
                break;
            }
        }
        if (logger_) {
            logger_->info("BluetoothDriver run succeeded");
        }
    } catch (...) {
        if (logger_) {
            try {
                throw;
            } catch (const std::exception& e) {
                logger_->error(std::string("BluetoothDriver run failed: ") + e.what());
            } catch (...) {
                logger_->error("BluetoothDriver run failed: unknown error");
            }
        }
        running_ = false;
        throw;
    }
}

void BluetoothDriver::stop() {
    if (logger_) {
        logger_->info("BluetoothDriver stop start");
    }
    try {
        running_ = false;
        if (logger_) {
            logger_->info("BluetoothDriver stop succeeded");
        }
    } catch (...) {
        if (logger_) {
            try {
                throw;
            } catch (const std::exception& e) {
                logger_->error(std::string("BluetoothDriver stop failed: ") + e.what());
            } catch (...) {
                logger_->error("BluetoothDriver stop failed: unknown error");
            }
        }
        throw;
    }
}

} // namespace device_reminder

