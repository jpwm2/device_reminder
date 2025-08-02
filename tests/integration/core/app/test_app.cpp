#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/ostream_sink.h>
#include <sstream>
#include <stdexcept>

#include "app/app.hpp"
#include "bluetooth_task/bluetooth_process.hpp"
#include "buzzer_task/buzzer_process.hpp"
#include "core/bluetooth_task/i_bluetooth_task.hpp"
#include "core/human_task/i_human_task.hpp"
#include "core/interfaces/i_handler.hpp"
#include "human_task/human_process.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/logger/logger.hpp"
#include "infra/process_operation/process_base/process_base.hpp"
#include "infra/process_operation/process_dispatcher/i_process_dispatcher.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/process_operation/process_receiver/i_process_receiver.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include "infra/watch_dog/i_watch_dog.hpp"
#include "main_task/main_process.hpp"

using ::testing::HasSubstr;
using ::testing::Return;
using ::testing::StrictMock;

namespace device_reminder {

// 各プロセスのモッククラス
class MockMainProcess : public device_reminder::IMainProcess {
public:
  MOCK_METHOD(int, run, (), (override));
  MOCK_METHOD(void, stop, (), (override));
};

class MockHumanProcess : public device_reminder::IHumanProcess {
public:
  MOCK_METHOD(int, run, (), (override));
  MOCK_METHOD(void, stop, (), (override));
};

class MockBluetoothProcess : public device_reminder::IBluetoothProcess {
public:
  MOCK_METHOD(int, run, (), (override));
  MOCK_METHOD(void, stop, (), (override));
};

class MockBuzzerProcess : public device_reminder::IBuzzerProcess {
public:
  MOCK_METHOD(int, run, (), (override));
  MOCK_METHOD(void, stop, (), (override));
};

// ロガーのモック
class MockLogger : public ILogger {
public:
  MOCK_METHOD(void, info, (const std::string &message), (override));
  MOCK_METHOD(void, error, (const std::string &message), (override));
  MOCK_METHOD(void, warn, (const std::string &), (override));
};

class MockProcessQueue : public IProcessQueue {
public:
  MOCK_METHOD(void, push, (std::shared_ptr<IProcessMessage> msg), (override));
  MOCK_METHOD(std::shared_ptr<IProcessMessage>, pop, (), (override));
  MOCK_METHOD(std::size_t, size, (), (const, override));
};

class MockProcessReceiver : public IProcessReceiver {
public:
  MOCK_METHOD(void, run, (), (override));
  MOCK_METHOD(void, stop, (), (override));
};

class MockProcessSender : public IProcessSender {
public:
  MOCK_METHOD(void, send, (), (override));
};

class MockProcessDispatcher : public IProcessDispatcher {
public:
  MOCK_METHOD(void, dispatch, (std::shared_ptr<IProcessMessage> msg),
              (override));
};

class MockFileLoader : public IFileLoader {
public:
  MOCK_METHOD(int, load_int, (const std::string &key), (const, override));
  MOCK_METHOD(std::string, load_string, (const std::string &key),
              (const, override));
  MOCK_METHOD(std::vector<std::string>, load_string_list,
              (const std::string &key), (const, override));
};

class MockWatchDog : public IWatchDog {
public:
  MOCK_METHOD(void, start, (), (override));
  MOCK_METHOD(void, stop, (), (override));
  MOCK_METHOD(void, kick, (), (override));
};

class MockHandler : public IHandler {
public:
  MOCK_METHOD(void, handle, (std::shared_ptr<IProcessMessage> msg), (override));
};

class MockHumanTask : public IHumanTask {
public:
  MOCK_METHOD(void, on_detecting, (const std::vector<std::string> &payload),
              (override));
  MOCK_METHOD(void, on_stopping, (const std::vector<std::string> &payload),
              (override));
  MOCK_METHOD(void, on_cooldown, (const std::vector<std::string> &payload),
              (override));
};

class MockBluetoothTask : public IBluetoothTask {
public:
  MOCK_METHOD(void, on_waiting, (const std::vector<std::string> &payload),
              (override));
};

class ProcessBaseAccessor : public ProcessBase {
public:
  ProcessBaseAccessor()
      : ProcessBase(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                    "Accessor") {}
  static void set_stop_flag(bool v) { g_stop_flag.store(v); }
};

} // namespace device_reminder

// テストケース
TEST(AppTest, Run_CallsAllTaskRunMethods) {
  using namespace device_reminder;

  // StrictMock を使うと予期しない呼び出しを検出できる
  auto main = std::make_unique<StrictMock<MockMainProcess>>();
  auto human = std::make_unique<StrictMock<MockHumanProcess>>();
  auto bluetooth = std::make_unique<StrictMock<MockBluetoothProcess>>();
  auto buzzer = std::make_unique<StrictMock<MockBuzzerProcess>>();
  auto logger = std::make_unique<StrictMock<MockLogger>>();

  // raw ポインタを退避して EXPECT_CALL に使う
  auto *main_ptr = main.get();
  auto *human_ptr = human.get();
  auto *bluetooth_ptr = bluetooth.get();
  auto *buzzer_ptr = buzzer.get();
  auto *logger_ptr = logger.get();

  // 各 run メソッドが1回ずつ呼び出されることを期待
  EXPECT_CALL(*main_ptr, run()).Times(1);
  EXPECT_CALL(*human_ptr, run()).Times(1);
  EXPECT_CALL(*bluetooth_ptr, run()).Times(1);
  EXPECT_CALL(*buzzer_ptr, run()).Times(1);
  EXPECT_CALL(*logger_ptr, info(testing::_)).Times(testing::AtLeast(1));

  App app(std::move(main), std::move(human), std::move(bluetooth),
          std::move(buzzer), std::move(logger));
  int result = app.run();

  // run の戻り値が 0 であることを確認
  EXPECT_EQ(result, 0);
}

// 例外が発生した場合にログに記録され、正しい戻り値が返るかをテスト
TEST(AppTest, Run_LogsAndReturns1OnStdException) {
  using namespace device_reminder;

  auto main = std::make_unique<StrictMock<MockMainProcess>>();
  auto human = std::make_unique<StrictMock<MockHumanProcess>>();
  auto bluetooth = std::make_unique<StrictMock<MockBluetoothProcess>>();
  auto buzzer = std::make_unique<StrictMock<MockBuzzerProcess>>();
  auto logger = std::make_unique<StrictMock<MockLogger>>();

  auto *logger_ptr = logger.get();

  // main_task の run が例外を投げる
  EXPECT_CALL(*main, run())
      .WillOnce(testing::Throw(std::runtime_error("test error")));
  // error ログが出力されることを期待
  EXPECT_CALL(*logger_ptr, error(testing::HasSubstr("test error")));

  App app(std::move(main), std::move(human), std::move(bluetooth),
          std::move(buzzer), std::move(logger));
  int result = app.run();

  EXPECT_EQ(result, 1);
}

TEST(AppTest, Run_LogsAndReturns2OnUnknownException) {
  using namespace device_reminder;

  auto main = std::make_unique<StrictMock<MockMainProcess>>();
  auto human = std::make_unique<StrictMock<MockHumanProcess>>();
  auto bluetooth = std::make_unique<StrictMock<MockBluetoothProcess>>();
  auto buzzer = std::make_unique<StrictMock<MockBuzzerProcess>>();
  auto logger = std::make_unique<StrictMock<MockLogger>>();

  auto *logger_ptr = logger.get();

  // main_task の run が不明な例外を投げる
  EXPECT_CALL(*main, run()).WillOnce(testing::Throw(42)); // 整数例外
  // error ログが出力されることを期待
  EXPECT_CALL(*logger_ptr, error(testing::HasSubstr("Unknown exception")));

  App app(std::move(main), std::move(human), std::move(bluetooth),
          std::move(buzzer), std::move(logger));
  int result = app.run();

  EXPECT_EQ(result, 2);
}

// プロセス実装を用いた統合テスト
TEST(AppIntegrationTest, RunSuccess) {
  using namespace device_reminder;

  ProcessBaseAccessor::set_stop_flag(true);

  auto null_sink = std::make_shared<spdlog::sinks::null_sink_st>();
  auto proc_spd = std::make_shared<spdlog::logger>("proc", null_sink);
  auto proc_logger = std::make_shared<Logger>(proc_spd);

  auto main_receiver = std::make_shared<StrictMock<MockProcessReceiver>>();
  auto main_sender = std::make_shared<StrictMock<MockProcessSender>>();
  auto main_queue = std::make_shared<StrictMock<MockProcessQueue>>();
  auto main_loader = std::make_shared<StrictMock<MockFileLoader>>();
  auto main_watchdog = std::make_shared<StrictMock<MockWatchDog>>();
  EXPECT_CALL(*main_loader, load_int("priority")).WillOnce(Return(0));
  EXPECT_CALL(*main_receiver, run()).Times(1);
  EXPECT_CALL(*main_receiver, stop()).Times(1);
  EXPECT_CALL(*main_watchdog, start()).Times(1);
  EXPECT_CALL(*main_watchdog, stop()).Times(1);
  auto main_proc =
      std::make_unique<MainProcess>(main_receiver, main_sender, main_queue,
                                    main_loader, proc_logger, main_watchdog);

  auto human_queue = std::make_shared<StrictMock<MockProcessQueue>>();
  auto human_receiver = std::make_shared<StrictMock<MockProcessReceiver>>();
  auto human_dispatcher = std::make_shared<StrictMock<MockProcessDispatcher>>();
  auto human_sender = std::make_shared<StrictMock<MockProcessSender>>();
  auto human_loader = std::make_shared<StrictMock<MockFileLoader>>();
  auto human_watchdog = std::make_shared<StrictMock<MockWatchDog>>();
  auto human_handler = std::make_shared<StrictMock<MockHandler>>();
  auto human_task = std::make_shared<StrictMock<MockHumanTask>>();
  EXPECT_CALL(*human_loader, load_int("priority")).WillOnce(Return(0));
  EXPECT_CALL(*human_receiver, run()).Times(1);
  EXPECT_CALL(*human_receiver, stop()).Times(1);
  EXPECT_CALL(*human_watchdog, start()).Times(1);
  EXPECT_CALL(*human_watchdog, stop()).Times(0);
  auto human_proc = std::make_unique<HumanProcess>(
      human_queue, human_receiver, human_dispatcher, human_sender, human_loader,
      proc_logger, human_watchdog, human_handler, human_task);

  auto bt_queue = std::make_shared<StrictMock<MockProcessQueue>>();
  auto bt_receiver = std::make_shared<StrictMock<MockProcessReceiver>>();
  auto bt_dispatcher = std::make_shared<StrictMock<MockProcessDispatcher>>();
  auto bt_sender = std::make_shared<StrictMock<MockProcessSender>>();
  auto bt_loader = std::make_shared<StrictMock<MockFileLoader>>();
  auto bt_watchdog = std::make_shared<StrictMock<MockWatchDog>>();
  auto bt_handler = std::make_shared<StrictMock<MockHandler>>();
  auto bt_task = std::make_shared<StrictMock<MockBluetoothTask>>();
  EXPECT_CALL(*bt_loader, load_int("priority")).WillOnce(Return(0));
  EXPECT_CALL(*bt_receiver, run()).Times(1);
  EXPECT_CALL(*bt_receiver, stop()).Times(1);
  EXPECT_CALL(*bt_watchdog, start()).Times(1);
  EXPECT_CALL(*bt_watchdog, stop()).Times(1);
  auto bt_proc = std::make_unique<BluetoothProcess>(
      bt_queue, bt_receiver, bt_dispatcher, bt_sender, bt_loader, proc_logger,
      bt_watchdog, bt_handler, bt_task);

  auto buzzer_receiver = std::make_shared<StrictMock<MockProcessReceiver>>();
  auto buzzer_sender = std::make_shared<StrictMock<MockProcessSender>>();
  auto buzzer_queue = std::make_shared<StrictMock<MockProcessQueue>>();
  auto buzzer_loader = std::make_shared<StrictMock<MockFileLoader>>();
  auto buzzer_watchdog = std::make_shared<StrictMock<MockWatchDog>>();
  EXPECT_CALL(*buzzer_loader, load_int("priority")).WillOnce(Return(0));
  EXPECT_CALL(*buzzer_receiver, run()).Times(1);
  EXPECT_CALL(*buzzer_receiver, stop()).Times(1);
  EXPECT_CALL(*buzzer_watchdog, start()).Times(1);
  EXPECT_CALL(*buzzer_watchdog, stop()).Times(1);
  auto buzzer_proc = std::make_unique<BuzzerProcess>(
      buzzer_receiver, buzzer_sender, buzzer_queue, buzzer_loader, proc_logger,
      buzzer_watchdog);

  std::ostringstream oss;
  auto sink = std::make_shared<spdlog::sinks::ostream_sink_st>(oss);
  auto app_spd = std::make_shared<spdlog::logger>("app", sink);
  auto app_logger = std::make_unique<Logger>(app_spd);

  App app(std::move(main_proc), std::move(human_proc), std::move(bt_proc),
          std::move(buzzer_proc), std::move(app_logger));
  int result = app.run();
  EXPECT_EQ(result, 0);
  EXPECT_THAT(oss.str(), HasSubstr("Completed successfully"));
}

// std::exception を投げた場合
TEST(AppIntegrationTest, RunStdException) {
  using namespace device_reminder;

  ProcessBaseAccessor::set_stop_flag(true);

  auto null_sink = std::make_shared<spdlog::sinks::null_sink_st>();
  auto proc_spd = std::make_shared<spdlog::logger>("proc", null_sink);
  auto proc_logger = std::make_shared<Logger>(proc_spd);

  auto main_receiver = std::make_shared<StrictMock<MockProcessReceiver>>();
  auto main_sender = std::make_shared<StrictMock<MockProcessSender>>();
  auto main_queue = std::make_shared<StrictMock<MockProcessQueue>>();
  auto main_loader = std::make_shared<StrictMock<MockFileLoader>>();
  auto main_watchdog = std::make_shared<StrictMock<MockWatchDog>>();
  EXPECT_CALL(*main_loader, load_int("priority")).WillOnce(Return(0));
  EXPECT_CALL(*main_receiver, run())
      .WillOnce(testing::Throw(std::runtime_error("test error")));
  EXPECT_CALL(*main_receiver, stop()).Times(0);
  EXPECT_CALL(*main_watchdog, start()).Times(1);
  EXPECT_CALL(*main_watchdog, stop()).Times(0);
  auto main_proc =
      std::make_unique<MainProcess>(main_receiver, main_sender, main_queue,
                                    main_loader, proc_logger, main_watchdog);

  auto human_queue = std::make_shared<StrictMock<MockProcessQueue>>();
  auto human_receiver = std::make_shared<StrictMock<MockProcessReceiver>>();
  auto human_dispatcher = std::make_shared<StrictMock<MockProcessDispatcher>>();
  auto human_sender = std::make_shared<StrictMock<MockProcessSender>>();
  auto human_loader = std::make_shared<StrictMock<MockFileLoader>>();
  auto human_watchdog = std::make_shared<StrictMock<MockWatchDog>>();
  auto human_handler = std::make_shared<StrictMock<MockHandler>>();
  auto human_task = std::make_shared<StrictMock<MockHumanTask>>();
  EXPECT_CALL(*human_loader, load_int("priority")).WillOnce(Return(0));
  auto human_proc = std::make_unique<HumanProcess>(
      human_queue, human_receiver, human_dispatcher, human_sender, human_loader,
      proc_logger, human_watchdog, human_handler, human_task);

  auto bt_queue = std::make_shared<StrictMock<MockProcessQueue>>();
  auto bt_receiver = std::make_shared<StrictMock<MockProcessReceiver>>();
  auto bt_dispatcher = std::make_shared<StrictMock<MockProcessDispatcher>>();
  auto bt_sender = std::make_shared<StrictMock<MockProcessSender>>();
  auto bt_loader = std::make_shared<StrictMock<MockFileLoader>>();
  auto bt_watchdog = std::make_shared<StrictMock<MockWatchDog>>();
  auto bt_handler = std::make_shared<StrictMock<MockHandler>>();
  auto bt_task = std::make_shared<StrictMock<MockBluetoothTask>>();
  EXPECT_CALL(*bt_loader, load_int("priority")).WillOnce(Return(0));
  auto bt_proc = std::make_unique<BluetoothProcess>(
      bt_queue, bt_receiver, bt_dispatcher, bt_sender, bt_loader, proc_logger,
      bt_watchdog, bt_handler, bt_task);

  auto buzzer_receiver = std::make_shared<StrictMock<MockProcessReceiver>>();
  auto buzzer_sender = std::make_shared<StrictMock<MockProcessSender>>();
  auto buzzer_queue = std::make_shared<StrictMock<MockProcessQueue>>();
  auto buzzer_loader = std::make_shared<StrictMock<MockFileLoader>>();
  auto buzzer_watchdog = std::make_shared<StrictMock<MockWatchDog>>();
  EXPECT_CALL(*buzzer_loader, load_int("priority")).WillOnce(Return(0));
  auto buzzer_proc = std::make_unique<BuzzerProcess>(
      buzzer_receiver, buzzer_sender, buzzer_queue, buzzer_loader, proc_logger,
      buzzer_watchdog);

  std::ostringstream oss;
  auto sink = std::make_shared<spdlog::sinks::ostream_sink_st>(oss);
  auto app_spd = std::make_shared<spdlog::logger>("app", sink);
  auto app_logger = std::make_unique<Logger>(app_spd);

  App app(std::move(main_proc), std::move(human_proc), std::move(bt_proc),
          std::move(buzzer_proc), std::move(app_logger));
  int result = app.run();
  EXPECT_EQ(result, 1);
  EXPECT_THAT(oss.str(), HasSubstr("std::exception"));
}

// 未知の例外を投げた場合
TEST(AppIntegrationTest, RunUnknownException) {
  using namespace device_reminder;

  ProcessBaseAccessor::set_stop_flag(true);

  auto null_sink = std::make_shared<spdlog::sinks::null_sink_st>();
  auto proc_spd = std::make_shared<spdlog::logger>("proc", null_sink);
  auto proc_logger = std::make_shared<Logger>(proc_spd);

  auto main_receiver = std::make_shared<StrictMock<MockProcessReceiver>>();
  auto main_sender = std::make_shared<StrictMock<MockProcessSender>>();
  auto main_queue = std::make_shared<StrictMock<MockProcessQueue>>();
  auto main_loader = std::make_shared<StrictMock<MockFileLoader>>();
  auto main_watchdog = std::make_shared<StrictMock<MockWatchDog>>();
  EXPECT_CALL(*main_loader, load_int("priority")).WillOnce(Return(0));
  EXPECT_CALL(*main_receiver, run()).WillOnce(testing::Throw(42));
  EXPECT_CALL(*main_receiver, stop()).Times(0);
  EXPECT_CALL(*main_watchdog, start()).Times(1);
  EXPECT_CALL(*main_watchdog, stop()).Times(0);
  auto main_proc =
      std::make_unique<MainProcess>(main_receiver, main_sender, main_queue,
                                    main_loader, proc_logger, main_watchdog);

  auto human_queue = std::make_shared<StrictMock<MockProcessQueue>>();
  auto human_receiver = std::make_shared<StrictMock<MockProcessReceiver>>();
  auto human_dispatcher = std::make_shared<StrictMock<MockProcessDispatcher>>();
  auto human_sender = std::make_shared<StrictMock<MockProcessSender>>();
  auto human_loader = std::make_shared<StrictMock<MockFileLoader>>();
  auto human_watchdog = std::make_shared<StrictMock<MockWatchDog>>();
  auto human_handler = std::make_shared<StrictMock<MockHandler>>();
  auto human_task = std::make_shared<StrictMock<MockHumanTask>>();
  EXPECT_CALL(*human_loader, load_int("priority")).WillOnce(Return(0));
  auto human_proc = std::make_unique<HumanProcess>(
      human_queue, human_receiver, human_dispatcher, human_sender, human_loader,
      proc_logger, human_watchdog, human_handler, human_task);

  auto bt_queue = std::make_shared<StrictMock<MockProcessQueue>>();
  auto bt_receiver = std::make_shared<StrictMock<MockProcessReceiver>>();
  auto bt_dispatcher = std::make_shared<StrictMock<MockProcessDispatcher>>();
  auto bt_sender = std::make_shared<StrictMock<MockProcessSender>>();
  auto bt_loader = std::make_shared<StrictMock<MockFileLoader>>();
  auto bt_watchdog = std::make_shared<StrictMock<MockWatchDog>>();
  auto bt_handler = std::make_shared<StrictMock<MockHandler>>();
  auto bt_task = std::make_shared<StrictMock<MockBluetoothTask>>();
  EXPECT_CALL(*bt_loader, load_int("priority")).WillOnce(Return(0));
  auto bt_proc = std::make_unique<BluetoothProcess>(
      bt_queue, bt_receiver, bt_dispatcher, bt_sender, bt_loader, proc_logger,
      bt_watchdog, bt_handler, bt_task);

  auto buzzer_receiver = std::make_shared<StrictMock<MockProcessReceiver>>();
  auto buzzer_sender = std::make_shared<StrictMock<MockProcessSender>>();
  auto buzzer_queue = std::make_shared<StrictMock<MockProcessQueue>>();
  auto buzzer_loader = std::make_shared<StrictMock<MockFileLoader>>();
  auto buzzer_watchdog = std::make_shared<StrictMock<MockWatchDog>>();
  EXPECT_CALL(*buzzer_loader, load_int("priority")).WillOnce(Return(0));
  auto buzzer_proc = std::make_unique<BuzzerProcess>(
      buzzer_receiver, buzzer_sender, buzzer_queue, buzzer_loader, proc_logger,
      buzzer_watchdog);

  std::ostringstream oss;
  auto sink = std::make_shared<spdlog::sinks::ostream_sink_st>(oss);
  auto app_spd = std::make_shared<spdlog::logger>("app", sink);
  auto app_logger = std::make_unique<Logger>(app_spd);

  App app(std::move(main_proc), std::move(human_proc), std::move(bt_proc),
          std::move(buzzer_proc), std::move(app_logger));
  int result = app.run();
  EXPECT_EQ(result, 2);
  EXPECT_THAT(oss.str(), HasSubstr("Unknown exception"));
}
