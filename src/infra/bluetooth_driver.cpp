#include "bluetooth_driver/bluetooth_driver.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace device_reminder {

BluetoothDriver::BluetoothDriver(ILogger& logger)
    : logger_{&logger} {}

std::vector<std::string> BluetoothDriver::scan() {
    std::vector<std::string> result;

    FILE* pipe = popen("hcitool scan", "r");
    if (!pipe) {
        if (logger_) logger_->error("failed to run hcitool scan");
        throw BluetoothDriverError("hcitool scan failed");
    }

    char line[256];
    bool first = true;
    while (fgets(line, sizeof(line), pipe)) {
        std::string str{line};
        if (first) { // skip header "Scanning ..."
            first = false;
            continue;
        }
        auto pos = str.find('\t');
        if (pos == std::string::npos) continue;
        std::string name = str.substr(pos + 1);
        while (!name.empty() && (name.back() == '\n' || name.back() == '\r')) {
            name.pop_back();
        }
        if (!name.empty()) {
            result.push_back(name);
            if (logger_) logger_->info("detected device: " + name);
        }
    }
    pclose(pipe);

    return result;
}

} // namespace device_reminder
