#pragma once
#include <memory>

namespace device_reminder {

class IThreadMessage;

class IThreadDispatcher {
public:
    virtual ~IThreadDispatcher() = default;
    virtual void dispatch(std::shared_ptr<IThreadMessage> msg) = 0;
};

} // namespace device_reminder
