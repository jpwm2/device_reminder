#pragma once

#include <cstdint>

namespace device_reminder {

/**
 * @brief Abstract interface for a message receiver working on its own thread.
 *        Implementations block waiting for incoming Message objects and
 *        typically forward them to a thread‑safe queue or handler.
 */
class IMessageReceiver {
public:
    virtual ~IMessageReceiver() = default;

    /**
     * @brief Thread entry point.  Call through std::thread (e.g. std::thread{receiver}).
     */
    virtual void operator()() = 0;

    /**
     * @brief Request graceful termination.  Must unblock the receive loop.
     */
    virtual void stop() = 0;

    /**
     * @brief True while the internal loop is running.
     */
    virtual bool running() const noexcept = 0;
};

} // namespace device_reminder