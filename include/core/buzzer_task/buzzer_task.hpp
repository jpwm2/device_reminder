#pragma once

#include "infra/thread_message_operation/i_thread_message.hpp"
#include "buzzer_task/i_buzzer_task.hpp"
#include "infra/buzzer_driver/i_buzzer_driver.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include "infra/file_loader/i_file_loader.hpp"

#include <memory>
#include <cstdint>

namespace device_reminder {

class BuzzerTask : public IBuzzerTask {
public:
    enum class State { WaitStart, Buzzing };

    BuzzerTask(std::shared_ptr<ILogger> logger,
               std::shared_ptr<IProcessSender> sender,
               std::shared_ptr<IFileLoader> file_loader,
               std::shared_ptr<IBuzzerDriver> driver);

    void run() override {}
    bool send_message(const IThreadMessage& msg) override;

    void onMessage(const IThreadMessage& msg);
    void on_waiting(const std::vector<std::string>& payload) override;
    void on_buzzing(const std::vector<std::string>& payload) override;

    State state() const noexcept { return state_; }

private:
    void startBuzzer();
    void stopBuzzer();

    std::shared_ptr<ILogger>        logger_;
    std::shared_ptr<IProcessSender> sender_;
    std::shared_ptr<IFileLoader>    file_loader_;
    std::shared_ptr<IBuzzerDriver>  driver_;
    State                           state_{State::WaitStart};
};

} // namespace device_reminder
