#include "main_task.hpp"
#include "message/i_message.hpp"

namespace device_reminder {

MainTask::MainTask() {
    // コンストラクタの初期化コード
}

MainTask::~MainTask() {
    // デストラクタの後始末コード
}

bool MainTask::send_message(const IMessage& msg) {

    return true;
}

void MainTask::run() {

}

} // namespace device_reminder
