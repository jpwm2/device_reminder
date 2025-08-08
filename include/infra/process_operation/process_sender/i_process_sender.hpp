#pragma once

namespace device_reminder {

class IProcessSender {
public:
    virtual ~IProcessSender() = default;
    virtual void send() = 0;
};

} // namespace device_reminder
