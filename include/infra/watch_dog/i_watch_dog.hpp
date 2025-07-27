#pragma once

namespace device_reminder {

class IWatchDog {
public:
    virtual ~IWatchDog() = default;

    virtual void start() = 0;
    virtual void stop()  = 0;
    virtual void kick()  = 0;
};

} // namespace device_reminder

