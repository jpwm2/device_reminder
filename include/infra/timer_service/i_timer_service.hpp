#pragma once

#include <cstdint>
#include "infra/process_operation/process_message/process_message.hpp"

namespace device_reminder {

class ITimerService {
public:
    virtual ~ITimerService() = default;

    virtual void start(uint32_t ms, const ProcessMessage& timeout_msg) = 0;
    virtual void stop() = 0;
    virtual bool active() const noexcept = 0;
};

} // namespace device_reminder
