#pragma once

namespace device_reminder {

class IBuzzerProcess {
public:
    virtual ~IBuzzerProcess() = default;
    virtual int run() = 0;
    virtual void stop() = 0;
};

} // namespace device_reminder
