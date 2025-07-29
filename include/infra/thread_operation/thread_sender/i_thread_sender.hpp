#pragma once
#include <memory>

namespace device_reminder {

class IThreadMessage; // forward
template <typename T>
class IThreadQueue;

class IThreadSender {
public:
    virtual ~IThreadSender() = default;
    virtual void send() = 0;
};

} // namespace device_reminder
