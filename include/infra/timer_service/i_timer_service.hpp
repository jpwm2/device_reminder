#pragma once

#include <memory>

namespace device_reminder {

class ITimerService {
public:
    virtual ~ITimerService() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
};

} // namespace device_reminder
