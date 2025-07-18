#include "buzzer_task.hpp"
#include <iostream>

namespace device_reminder {

BuzzerTask::BuzzerTask() {
    // 初期化処理
}

BuzzerTask::~BuzzerTask() {
    // 後始末処理
}

void BuzzerTask::run() {
}

bool BuzzerTask::send_message(const IMessage& msg) {
    return true;
}

} // namespace device_reminder
