#pragma once

#include <stdexcept>
#include <string>
#include <vector>

namespace device_reminder {

class BluetoothDriverError : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

class IBluetoothDriver {
public:
  virtual ~IBluetoothDriver() = default;

  /// Bluetoothスキャンを開始し、検知と通知処理を行う
  virtual void run() = 0;

  /// Bluetoothスキャン処理を停止する
  virtual void stop() = 0;

  /// ペアリングに成功したデバイス一覧を返す
  virtual const std::vector<std::string> &paired_devices() const = 0;
};

} // namespace device_reminder
