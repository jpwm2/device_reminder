#pragma once

#include <spdlog/logger.h>
#include <memory>
#include <string>

namespace device_reminder {

class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void info(const std::string& msg) = 0;
    virtual void error(const std::string& msg) = 0;
    virtual void warn(const std::string& msg) = 0;
};

class Logger : public ILogger {
public:
    explicit Logger(std::shared_ptr<spdlog::logger> logger);

    void info(const std::string& msg) override;
    void error(const std::string& msg) override;
    void warn(const std::string& msg) override;

private:
    std::shared_ptr<spdlog::logger> logger_;
};

} // namespace device_reminder
