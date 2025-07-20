#pragma once
#include "infra/logger/i_logger.hpp"
#include <memory>

namespace device_reminder {

class SpeakerDriver {
public:
    explicit SpeakerDriver(std::shared_ptr<ILogger> logger = nullptr);
    ~SpeakerDriver();
    void set_enabled(bool enable);

private:
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder