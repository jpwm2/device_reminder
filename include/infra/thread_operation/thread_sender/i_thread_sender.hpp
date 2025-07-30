#pragma once
#include <memory>

namespace device_reminder {

class IThreadMessage; // forward declaration
class IThreadQueue;

class IThreadSender {
public:
    virtual ~IThreadSender() = default;
    virtual void send() = 0;
};

} // namespace device_reminder
