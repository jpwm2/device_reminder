#pragma once

#include "di/logger_injector.hpp"
#include "core/main_task/main_task.hpp"
#include "core/human_task/human_task.hpp"
#include "core/bluetooth_task/bluetooth_task.hpp"
#include "core/buzzer_task/buzzer_task.hpp"
#include "infra/timer_service/timer_service.hpp"
#include "infra/pir_driver/pir_driver.hpp"
#include "infra/gpio_driver/gpio_driver.hpp"
#include "infra/buzzer_driver/buzzer_driver.hpp"
#include "infra/bluetooth_driver/bluetooth_driver.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include "thread_message_operation/thread_message_queue.hpp"
#include "thread_message_operation/i_message_queue.hpp"

#include <tuple>
#include <boost/di.hpp>

namespace di = boost::di;

namespace device_reminder {

namespace detail {

// 任意個の tuple を展開して、全injectorをまとめるユーティリティ
template <typename... Tuples>
auto merge_injectors(Tuples&&... tuples) {
    auto combined = std::tuple_cat(std::forward<Tuples>(tuples)...);
    return std::apply([](auto&&... injectors) {
        return di::make_injector(std::forward<decltype(injectors)>(injectors)...);
    }, std::move(combined));
}

} // namespace detail


inline auto make_app_injector() {
    auto logger = make_logger_injector();
    auto core = std::tuple{
        di::bind<IMainTask>.to<MainTask>(),
        di::bind<IHumanTask>.to<HumanTask>(),
        di::bind<IBluetoothTask>.to<BluetoothTask>(),
        di::bind<IBuzzerTask>.to<BuzzerTask>(),
        di::bind<ITimerService>.to<TimerService>(),
        di::bind<IThreadMessageQueue>.to<ThreadMessageQueue>(),
        di::bind<IProcessSender>.to([] { return std::shared_ptr<IProcessSender>{}; }),
        di::bind<IGPIODriver>.to<GPIODriver>(),
        di::bind<IBuzzerDriver>.to<BuzzerDriver>(),
        di::bind<IPIRDriver>.to<PIRDriver>(),
        di::bind<IBluetoothDriver>.to<BluetoothDriver>()
    };

    return detail::merge_injectors(
        logger,
        core
    );
}

}
