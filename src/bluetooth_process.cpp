#include "bluetooth_process.hpp"
#include "message/i_message.hpp"

namespace device_reminder {

BluetoothProcess::BluetoothProcess() {
    // コンストラクタの初期化コード
}

BluetoothProcess::~BluetoothProcess() {
    // デストラクタの後始末コード
}

void BluetoothProcess::run() {
}

bool BluetoothProcess::send_message(const IMessage& msg) {
    return true;
}

} // namespace device_reminder
