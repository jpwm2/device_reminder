#pragma once

#include <memory>

namespace device_reminder {

class IProcessMessage;

class IHandler {
public:
    virtual ~IHandler() = default;
    virtual void handle(std::shared_ptr<IProcessMessage> msg) = 0;
};

} // namespace device_reminder
