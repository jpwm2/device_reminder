#pragma once

namespace device_reminder {
class IThreadReceiver {
public:
    virtual ~IThreadReceiver() = default;
    virtual void run() = 0;
    virtual void stop() = 0;
};
}
