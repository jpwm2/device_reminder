#pragma once

namespace device_reminder {

class IHumanProcess {
public:
    virtual ~IHumanProcess() = default;
    virtual void run() = 0;
    virtual void stop() = 0;
};

} // namespace device_reminder
