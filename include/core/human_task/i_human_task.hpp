#pragma once

#include <string>
#include <vector>

namespace device_reminder {

class IHumanTask {
public:
    virtual ~IHumanTask() = default;

    virtual void on_detecting(const std::vector<std::string>& payload) = 0;
    virtual void on_stopping(const std::vector<std::string>& payload) = 0;
    virtual void on_cooldown(const std::vector<std::string>& payload) = 0;
};

} // namespace device_reminder
