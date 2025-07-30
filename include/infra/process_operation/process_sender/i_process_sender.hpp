#pragma once

#include <memory>

namespace device_reminder {

class IProcessQueue;
class IProcessMessage;

class IProcessSender {
public:
  virtual ~IProcessSender() = default;
  virtual void send() = 0;
};

} // namespace device_reminder
