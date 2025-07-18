#pragma once

namespace device_reminder {

class SpeakerDriver {
public:
    SpeakerDriver();
    ~SpeakerDriver();
    void set_enabled(bool enable);
};

} // namespace device_reminder