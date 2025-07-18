#include "human_task.hpp"

namespace device_reminder {

HumanTask::HumanTask() {
    // コンストラクタの初期化コード
}

HumanTask::~HumanTask() {
    // デストラクタの後始末コード
}

void HumanTask::run() {
}

bool HumanTask::send_message(const IMessage& msg) {
    return true;
}

} // namespace device_reminder
