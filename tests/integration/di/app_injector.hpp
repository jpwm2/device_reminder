#pragma once

#include <gmock/gmock.h>
#include <boost/di.hpp>

#include "core/main_task/i_main_task.hpp"
#include "core/human_task/i_human_task.hpp"
#include "core/bluetooth_task/i_bluetooth_task.hpp"
#include "core/buzzer_task/i_buzzer_task.hpp"
#include "infra/logger/i_logger.hpp"

namespace di = boost::di;
namespace device_reminder {

class MockMainTask;
class MockHumanTask;
class MockBluetoothTask;
class MockBuzzerTask;
class MockLogger;

inline MockMainTask* g_mock_main_task = nullptr;
inline MockHumanTask* g_mock_human_task = nullptr;
inline MockBluetoothTask* g_mock_bluetooth_task = nullptr;
inline MockBuzzerTask* g_mock_buzzer_task = nullptr;
inline MockLogger* g_mock_logger = nullptr;

class MockMainTask : public IMainTask {
public:
    MockMainTask() { g_mock_main_task = this; }
    MOCK_METHOD(void, run, (const IThreadMessage& msg), (override));
    MOCK_METHOD(void, on_waiting_for_human, (const std::vector<std::string>&), (override));
    MOCK_METHOD(void, on_response_to_buzzer_task, (const std::vector<std::string>&), (override));
    MOCK_METHOD(void, on_response_to_human_task, (const std::vector<std::string>&), (override));
    MOCK_METHOD(void, on_cooldown, (const std::vector<std::string>&), (override));
    MOCK_METHOD(void, on_waiting_for_second_response, (const std::vector<std::string>&), (override));
};

class MockHumanTask : public IHumanTask {
public:
    MockHumanTask() { g_mock_human_task = this; }
    MOCK_METHOD(void, on_detecting, (const std::vector<std::string>&), (override));
    MOCK_METHOD(void, on_stopping, (const std::vector<std::string>&), (override));
    MOCK_METHOD(void, on_cooldown, (const std::vector<std::string>&), (override));
};

class MockBluetoothTask : public IBluetoothTask {
public:
    MockBluetoothTask() { g_mock_bluetooth_task = this; }
    MOCK_METHOD(void, on_waiting, (const std::vector<std::string>&), (override));
};

class MockBuzzerTask : public IBuzzerTask {
public:
    MockBuzzerTask() { g_mock_buzzer_task = this; }
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(bool, send_message, (const IThreadMessage&), (override));
    MOCK_METHOD(void, on_waiting, (const std::vector<std::string>&), (override));
    MOCK_METHOD(void, on_buzzing, (const std::vector<std::string>&), (override));
};

class MockLogger : public ILogger {
public:
    MockLogger() { g_mock_logger = this; }
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};


inline auto make_app_injector() {
    return di::make_injector(
        di::bind<IMainTask>.to([] {
            return std::unique_ptr<IMainTask>(g_mock_main_task = new testing::StrictMock<MockMainTask>());
        }),
        di::bind<IHumanTask>.to([] {
            return std::unique_ptr<IHumanTask>(g_mock_human_task = new testing::StrictMock<MockHumanTask>());
        }),
        di::bind<IBluetoothTask>.to([] {
            return std::unique_ptr<IBluetoothTask>(g_mock_bluetooth_task = new testing::StrictMock<MockBluetoothTask>());
        }),
        di::bind<IBuzzerTask>.to([] {
            return std::unique_ptr<IBuzzerTask>(g_mock_buzzer_task = new testing::StrictMock<MockBuzzerTask>());
        }),
        di::bind<ILogger>.to([] {
            return std::unique_ptr<ILogger>(g_mock_logger = new testing::StrictMock<MockLogger>());
        })
    );
}

} // namespace device_reminder
