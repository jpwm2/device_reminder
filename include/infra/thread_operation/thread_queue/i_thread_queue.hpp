#pragma once
#include <optional>
#include <cstddef>

namespace device_reminder {

template <typename T>
class IThreadQueue {
public:
    virtual ~IThreadQueue() = default;

    virtual void push(const T& item) = 0;
    virtual std::optional<T> pop(int timeout_ms = -1) = 0;
    virtual bool empty() const = 0;
    virtual size_t size() const = 0;
    virtual void clear() = 0;
};

} // namespace device_reminder
