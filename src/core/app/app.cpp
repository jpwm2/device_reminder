#include "app/app.hpp"

#include <array>
#include <string>
#include <utility>

namespace device_reminder {

App::App(std::unique_ptr<IProcess> main_process,
         std::unique_ptr<IProcess> human_process,
         std::unique_ptr<IProcess> bluetooth_process,
         std::unique_ptr<IProcess> buzzer_process,
         std::unique_ptr<ILogger> logger)
    : main_process_(std::move(main_process))
    , human_process_(std::move(human_process))
    , bluetooth_process_(std::move(bluetooth_process))
    , buzzer_process_(std::move(buzzer_process))
    , logger_(std::move(logger)) {}

void App::run() {
    logger_->info("App.run 開始");
    try {
        struct ProcessInfo {
            const char* name;
            IProcess* process;
        };
        std::array<ProcessInfo, 4> processes = {{
            {"main_process", main_process_.get()},
            {"human_process", human_process_.get()},
            {"bluetooth_process", bluetooth_process_.get()},
            {"buzzer_process", buzzer_process_.get()},
        }};

        for (auto& info : processes) {
            logger_->info(std::string(info.name) + " 起動開始");
            info.process->run();
            logger_->info(std::string(info.name) + " 起動完了");
        }

        logger_->info("App.run 成功（全タスク起動）");
    } catch (const std::exception& e) {
        logger_->error(std::string("App.run 失敗: ") + e.what());
        throw;
    } catch (...) {
        logger_->error("App.run 失敗: 不明な例外");
        throw;
    }
}

} // namespace device_reminder
