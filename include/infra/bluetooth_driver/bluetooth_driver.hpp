#pragma once

#include <memory>
#include <atomic>

namespace device_reminder {

class IBluetoothScanner;
class IBluetoothPairer;
class ILogger;

class IBluetoothDriver {
public:
    virtual ~IBluetoothDriver() = default;
    virtual void run() = 0;
    virtual void stop() = 0;
};

class BluetoothDriver : public IBluetoothDriver {
public:
    BluetoothDriver(std::shared_ptr<IBluetoothScanner> scanner,
                    std::shared_ptr<IBluetoothPairer> pairer,
                    std::shared_ptr<ILogger> logger);

    void run() override;
    void stop() override;

private:
    std::shared_ptr<IBluetoothScanner> scanner_{};
    std::shared_ptr<IBluetoothPairer> pairer_{};
    std::shared_ptr<ILogger> logger_{};
    std::atomic<bool> running_{false};
};

} // namespace device_reminder

