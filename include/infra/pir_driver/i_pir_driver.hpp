#pragma once

namespace device_reminder {

class IPIRDriver {
public:
    virtual ~IPIRDriver() = default;

    // PIR センサーの監視を開始する
    virtual void run() = 0;

    // PIR センサーの監視を停止する
    virtual void stop() = 0;

};

} // namespace device_reminder
