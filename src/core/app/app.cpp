#include "app/app.hpp"

#include <array>
#include <string>

namespace device_reminder {

App::App(IProcess& main_process,
         IProcess& human_process,
         IProcess& bluetooth_process,
         IProcess& buzzer_process,
         ILogger& logger)
    : main_process_(main_process)
    , human_process_(human_process)
    , bluetooth_process_(bluetooth_process)
    , buzzer_process_(buzzer_process)
    , logger_(logger) {}

void App::run() {
    logger_.info("App.run 開始");
    try {
        struct ProcessInfo {
            const char* name;
            IProcess& process;
        };
        std::array<ProcessInfo, 4> processes = {{
            {"main_process", main_process_},
            {"human_process", human_process_},
            {"bluetooth_process", bluetooth_process_},
            {"buzzer_process", buzzer_process_},
        }};

        for (auto& info : processes) {
            logger_.info(std::string(info.name) + " 起動開始");
            info.process.run();
            logger_.info(std::string(info.name) + " 起動完了");
        }

        logger_.info("App.run 成功（全タスク起動）");
    } catch (const std::exception& e) {
        logger_.error(std::string("App.run 失敗: ") + e.what());
        throw;
    } catch (...) {
        logger_.error("App.run 失敗: 不明な例外");
        throw;
    }
}

} // namespace device_reminder
