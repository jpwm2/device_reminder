#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "core/app_builder/app_builder.hpp"
#include "core/main_task/main_task.hpp"
#include "core/human_task/human_task.hpp"
#include "core/bluetooth_task/bluetooth_task.hpp"
#include "core/buzzer_task/buzzer_task.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/timer_service/i_timer_service.hpp"
#include "infra/pir_driver/i_pir_driver.hpp"
#include "infra/bluetooth_driver/i_bluetooth_driver.hpp"
#include "infra/buzzer_driver/i_buzzer_driver.hpp"
#include "app/app.hpp"

#include <boost/di.hpp>
#include <memory>
#include <iostream>

using ::testing::StrictMock;
using ::testing::AtLeast;
using ::testing::_;

namespace device_reminder {

// ----------------- モック定義 -----------------
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

class MockProcessSender : public IProcessSender {
public:
    MOCK_METHOD(void, send, (), (override));
};

class MockFileLoader : public IFileLoader {
public:
    MOCK_METHOD(int, load_int, (const std::string& key), (const, override));
    MOCK_METHOD(std::string, load_string, (const std::string& key), (const, override));
    MOCK_METHOD(std::vector<std::string>, load_string_list, (const std::string& key), (const, override));
};

class MockTimerService : public ITimerService {
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
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

// ----------------- ダミープロセスクラス -----------------
class DummyMainProcess : public IMainProcess {
public:
    explicit DummyMainProcess(std::unique_ptr<IMainTask> task) : task_(std::move(task)) {}
    int run() override { return 0; }
    void stop() override {}
private:
    std::unique_ptr<IMainTask> task_;
};

class DummyHumanProcess : public IHumanProcess {
public:
    explicit DummyHumanProcess(std::unique_ptr<IHumanTask> task) : task_(std::move(task)) {}
    int run() override { return 0; }
    void stop() override {}
private:
    std::unique_ptr<IHumanTask> task_;
};

class DummyBluetoothProcess : public IBluetoothProcess {
public:
    explicit DummyBluetoothProcess(std::unique_ptr<IBluetoothTask> task) : task_(std::move(task)) {}
    int run() override { return 0; }
    void stop() override {}
private:
    std::unique_ptr<IBluetoothTask> task_;
};

class DummyBuzzerProcess : public IBuzzerProcess {
public:
    explicit DummyBuzzerProcess(std::unique_ptr<IBuzzerTask> task) : task_(std::move(task)) {}
    int run() override { return 0; }
    void stop() override {}
private:
    std::unique_ptr<IBuzzerTask> task_;
};

// ----------------- グローバル変数 -----------------
std::shared_ptr<ILogger> g_logger;
bool g_inject_failure = false;

namespace di = boost::di;

// ----------------- テスト用インジェクタ -----------------
inline auto make_app_injector() {
    auto logger_provider = []() -> std::shared_ptr<ILogger> {
        if (g_inject_failure) {
            throw std::runtime_error("inject fail");
        }
        return g_logger;
    };

    return di::make_injector(
        di::bind<IMainTask>.to<MainTask>(),
        di::bind<IHumanTask>.to<HumanTask>(),
        di::bind<IBluetoothTask>.to<BluetoothTask>(),
        di::bind<IBuzzerTask>.to<BuzzerTask>(),
        di::bind<ILogger>.to(logger_provider),
        di::bind<IProcessSender>.to<MockProcessSender>(),
        di::bind<IFileLoader>.to<MockFileLoader>(),
        di::bind<ITimerService>.to<MockTimerService>(),
        di::bind<IPIRDriver>.to<MockPIRDriver>(),
        di::bind<IBluetoothDriver>.to<MockBluetoothDriver>(),
        di::bind<IBuzzerDriver>.to<MockBuzzerDriver>()
    );
}

// ----------------- AppBuilder::build 実装 -----------------
std::unique_ptr<App> AppBuilder::build() {
    auto injector = make_app_injector();

    std::unique_ptr<IMainTask> main_task;
    std::unique_ptr<IHumanTask> human_task;
    std::unique_ptr<IBluetoothTask> bluetooth_task;
    std::unique_ptr<IBuzzerTask> buzzer_task;
    std::unique_ptr<ILogger> logger;

    try {
        main_task = injector.create<std::unique_ptr<IMainTask>>();
        human_task = injector.create<std::unique_ptr<IHumanTask>>();
        bluetooth_task = injector.create<std::unique_ptr<IBluetoothTask>>();
        buzzer_task = injector.create<std::unique_ptr<IBuzzerTask>>();
        logger = injector.create<std::unique_ptr<ILogger>>();

        auto main_proc = std::make_unique<DummyMainProcess>(std::move(main_task));
        auto human_proc = std::make_unique<DummyHumanProcess>(std::move(human_task));
        auto bt_proc = std::make_unique<DummyBluetoothProcess>(std::move(bluetooth_task));
        auto buzzer_proc = std::make_unique<DummyBuzzerProcess>(std::move(buzzer_task));

        return std::make_unique<App>(
            std::move(main_proc),
            std::move(human_proc),
            std::move(bt_proc),
            std::move(buzzer_proc),
            std::move(logger));
    } catch (const std::exception& e) {
        if (logger) {
            logger->error(std::string("[AppBuilder::build] std::exception: ") + e.what());
        } else {
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

// ------------------ テスト ------------------
TEST(AppBuilderIntegrationTest, BuildSuccess) {
    using namespace device_reminder;
    g_inject_failure = false;

    auto logger = std::make_shared<StrictMock<MockLogger>>();
    g_logger = logger;

    EXPECT_CALL(*logger, info(_)).Times(AtLeast(4));

    AppBuilder builder;
    auto app = builder.build();
    EXPECT_NE(app, nullptr);
}

TEST(AppBuilderIntegrationTest, BuildThrowsWhenDependencyFails) {
    using namespace device_reminder;
    g_inject_failure = true;

    auto logger = std::make_shared<StrictMock<MockLogger>>();
    g_logger = logger;

    AppBuilder builder;
    EXPECT_THROW(builder.build(), std::runtime_error);
}

