#pragma once
#include <cstddef>
#include <memory>

namespace device_reminder {

class IThreadMessage;

class IThreadQueue {
public:
  virtual ~IThreadQueue() = default;

  virtual void push(std::shared_ptr<IThreadMessage> msg) = 0;
  virtual std::shared_ptr<IThreadMessage> pop() = 0;
  virtual size_t size() const = 0;
};

} // namespace device_reminder
