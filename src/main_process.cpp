#include "main_process.hpp"
#include "message/i_message.hpp"

namespace device_reminder {

MainProcess::MainProcess() {
    // コンストラクタの初期化コード
}

MainProcess::~MainProcess() {
    // デストラクタの後始末コード
}

bool MainProcess::send_message(const IMessage& msg) {

    return true;
}

void MainProcess::run() {

}

} // namespace device_reminder
