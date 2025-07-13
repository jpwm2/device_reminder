#include "human_process.hpp"

namespace device_reminder {

HumanProcess::HumanProcess() {
    // コンストラクタの初期化コード
}

HumanProcess::~HumanProcess() {
    // デストラクタの後始末コード
}

void HumanProcess::run() {
}

bool HumanProcess::send_message(const IMessage& msg) {
    return true;
}

} // namespace device_reminder
