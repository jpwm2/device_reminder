#include <iostream>

#include "app_builder/app_builder.hpp"

#include "app/app.hpp"
#include "di/app_injector.hpp"  // make_app_injector()を想定

namespace device_reminder {

std::unique_ptr<App> AppBuilder::build() {
    auto injector = make_app_injector();

    
    std::unique_ptr<IMainTask> main_process;
    std::unique_ptr<IHumanTask> human_process;
    std::unique_ptr<IBluetoothTask> bluetooth_process;
    std::unique_ptr<IBuzzerTask> buzzer_process;
    std::unique_ptr<ILogger> logger;

    try{
        main_process = injector.create<std::unique_ptr<IMainTask>>();
        human_process = injector.create<std::unique_ptr<IHumanTask>>();
        bluetooth_process = injector.create<std::unique_ptr<IBluetoothTask>>();
        buzzer_process = injector.create<std::unique_ptr<IBuzzerTask>>();
        logger = injector.create<std::unique_ptr<ILogger>>();

        return std::make_unique<App>(std::move(main_process),
                                    std::move(human_process),
                                    std::move(bluetooth_process),
                                    std::move(buzzer_process),
                                    std::move(logger));

    } catch (const std::exception& e) {
        if (logger) {
            logger->error(std::string("[AppBuilder::build] std::exception: ") + e.what());
        } else {
            // fallback: 標準エラー出力
            std::cerr << "[AppBuilder::build] std::exception: " << e.what() << std::endl;
        }
        throw;
    } catch (...) {
        if (logger) {
            logger->error("[AppBuilder::build] Unknown exception caught.");
        } else {
            std::cerr << "[AppBuilder::build] Unknown exception caught." << std::endl;
        }
        throw;
    }


}

} // namespace device_reminder
