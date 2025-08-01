#pragma once

#include <gmock/gmock.h>
#include <boost/di.hpp>
#include <memory>
#include <vector>
#include <string>

#include "core/main_task/main_task.hpp"
#include "core/human_task/human_task.hpp"
#include "core/bluetooth_task/bluetooth_task.hpp"
#include "core/buzzer_task/buzzer_task.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/pir_driver/i_pir_driver.hpp"
#include "infra/bluetooth_driver/i_bluetooth_driver.hpp"
#include "infra/buzzer_driver/i_buzzer_driver.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include "infra/timer_service/i_timer_service.hpp"

namespace di = boost::di;
namespace device_reminder {

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

class MockPIRDriver : public IPIRDriver {
public:
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};

class MockBluetoothDriver : public IBluetoothDriver {
public:
    MOCK_METHOD(std::vector<std::string>, scan, (), (override));
};

class MockBuzzerDriver : public IBuzzerDriver {
public:
    MOCK_METHOD(void, on, (), (override));
    MOCK_METHOD(void, off, (), (override));
};

class MockFileLoader : public IFileLoader {
public:
    MOCK_METHOD(int, load_int, (const std::string&), (const, override));
    MOCK_METHOD(std::string, load_string, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<std::string>, load_string_list, (const std::string&), (const, override));
};

class MockProcessSender : public IProcessSender {
public:
    MOCK_METHOD(void, send, (), (override));
};

class MockTimerService : public ITimerService {
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};

inline MockLogger* g_mock_logger = nullptr;
inline MockPIRDriver* g_mock_pir_driver = nullptr;
inline MockBluetoothDriver* g_mock_bt_driver = nullptr;
inline MockBuzzerDriver* g_mock_buzzer_driver = nullptr;
inline MockFileLoader* g_mock_file_loader = nullptr;
inline MockProcessSender* g_mock_process_sender = nullptr;
inline MockTimerService* g_mock_timer_service = nullptr;

inline auto make_app_injector() {
    return di::make_injector(
        di::bind<IMainTask>.to<MainTask>(),
        di::bind<IHumanTask>.to<HumanTask>(),
        di::bind<IBluetoothTask>.to<BluetoothTask>(),
        di::bind<IBuzzerTask>.to<BuzzerTask>(),
        di::bind<ILogger>.to([] {
            return std::shared_ptr<ILogger>(g_mock_logger = new testing::StrictMock<MockLogger>());
        }),
        di::bind<IPIRDriver>.to([] {
            return std::shared_ptr<IPIRDriver>(g_mock_pir_driver = new testing::StrictMock<MockPIRDriver>());
        }),
        di::bind<IBluetoothDriver>.to([] {
            return std::shared_ptr<IBluetoothDriver>(g_mock_bt_driver = new testing::StrictMock<MockBluetoothDriver>());
        }),
        di::bind<IBuzzerDriver>.to([] {
            return std::shared_ptr<IBuzzerDriver>(g_mock_buzzer_driver = new testing::StrictMock<MockBuzzerDriver>());
        }),
        di::bind<IFileLoader>.to([] {
            return std::shared_ptr<IFileLoader>(g_mock_file_loader = new testing::StrictMock<MockFileLoader>());
        }),
        di::bind<IProcessSender>.to([] {
            return std::shared_ptr<IProcessSender>(g_mock_process_sender = new testing::StrictMock<MockProcessSender>());
        }),
        di::bind<ITimerService>.to([] {
            return std::shared_ptr<ITimerService>(g_mock_timer_service = new testing::StrictMock<MockTimerService>());
        })
    );
}

} // namespace device_reminder

