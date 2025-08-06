#pragma once
#include <memory>

namespace device_reminder {

class IMessage;

class IThreadDispatcher {
public:
    virtual ~IThreadDispatcher() = default;
    virtual void dispatch(std::shared_ptr<IMessage> msg) = 0;
};

} // namespace device_reminder
