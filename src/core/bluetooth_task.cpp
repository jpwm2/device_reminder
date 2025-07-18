#include "bluetooth_task.hpp"
#include "message/i_message.hpp"

namespace device_reminder {

BluetoothTask::BluetoothTask() {
    // コンストラクタの初期化コード
}

BluetoothTask::~BluetoothTask() {
    // デストラクタの後始末コード
}

void BluetoothTask::run() {
}

bool BluetoothTask::send_message(const IMessage& msg) {
    return true;
}

} // namespace device_reminder
