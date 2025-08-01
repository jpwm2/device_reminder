#pragma once

#include <string>

namespace device_reminder {

class ILogger {
public:
    virtual ~ILogger() = default;

    virtual void info(const std::string& msg) = 0;
    virtual void error(const std::string& msg) = 0;
    virtual void warn(const std::string& msg) = 0;
};

} // namespace device_reminder
