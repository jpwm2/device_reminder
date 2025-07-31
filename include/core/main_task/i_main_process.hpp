#pragma once

namespace device_reminder {

class IMainProcess {
public:
    virtual ~IMainProcess() = default;
    virtual void run() = 0;
    virtual void stop() = 0;
};

} // namespace device_reminder
