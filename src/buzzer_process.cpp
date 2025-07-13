#include "buzzer_process.hpp"
#include <iostream>

namespace device_reminder {

BuzzerProcess::BuzzerProcess() {
    // 初期化処理
}

BuzzerProcess::~BuzzerProcess() {
    // 後始末処理
}

void BuzzerProcess::run() {
}

bool BuzzerProcess::send_message(const IMessage& msg) {
    return true;
}

} // namespace device_reminder
