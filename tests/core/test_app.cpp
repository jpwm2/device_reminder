#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "app/app.hpp"

using ::testing::Return;
using ::testing::StrictMock;

namespace device_reminder {

// 各プロセスのモッククラス
class MockMainTask : public device_reminder::IMainTask {
public:
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(bool, send_message, (const device_reminder::IMessage& msg), (override));
};

class MockHumanTask : public device_reminder::IHumanTask {
public:
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(bool, send_message, (const device_reminder::IMessage& msg), (override));
};

class MockBluetoothTask : public device_reminder::IBluetoothTask {
public:
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(bool, send_message, (const device_reminder::IMessage& msg), (override));
};


class MockBuzzerTask : public device_reminder::IBuzzerTask {
public:
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(bool, send_message, (const device_reminder::IMessage& msg), (override));
};

// ロガーのモック
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string& message), (override));
    MOCK_METHOD(void, error, (const std::string& message), (override));
};

} // namespace device_reminder

// テストケース
TEST(AppTest, Run_CallsAllTaskRunMethods) {
    using namespace device_reminder;

    // StrictMock を使うと予期しない呼び出しを検出できる
    auto main = std::make_unique<StrictMock<MockMainTask>>();
    auto human = std::make_unique<StrictMock<MockHumanTask>>();
    auto bluetooth = std::make_unique<StrictMock<MockBluetoothTask>>();
    auto buzzer = std::make_unique<StrictMock<MockBuzzerTask>>();
    auto logger = std::make_unique<StrictMock<MockLogger>>();

    // raw ポインタを退避して EXPECT_CALL に使う
    auto* main_ptr = main.get();
    auto* human_ptr = human.get();
    auto* bluetooth_ptr = bluetooth.get();
    auto* buzzer_ptr = buzzer.get();
    auto* logger_ptr = logger.get();

    // 各 run メソッドが1回ずつ呼び出されることを期待
    EXPECT_CALL(*main_ptr, run()).Times(1);
    EXPECT_CALL(*human_ptr, run()).Times(1);
    EXPECT_CALL(*bluetooth_ptr, run()).Times(1);
    EXPECT_CALL(*buzzer_ptr, run()).Times(1);
    EXPECT_CALL(*logger_ptr, info(testing::_)).Times(testing::AtLeast(1));

    App app(std::move(main), std::move(human), std::move(bluetooth), std::move(buzzer), std::move(logger));
    int result = app.run();

    // run の戻り値が 0 であることを確認
    EXPECT_EQ(result, 0);
}


// 例外が発生した場合にログに記録され、正しい戻り値が返るかをテスト
TEST(AppTest, Run_LogsAndReturns1OnStdException) {
    using namespace device_reminder;

    auto main = std::make_unique<StrictMock<MockMainTask>>();
    auto human = std::make_unique<StrictMock<MockHumanTask>>();
    auto bluetooth = std::make_unique<StrictMock<MockBluetoothTask>>();
    auto buzzer = std::make_unique<StrictMock<MockBuzzerTask>>();
    auto logger = std::make_unique<StrictMock<MockLogger>>();

    auto* logger_ptr = logger.get();

    // main_task の run が例外を投げる
    EXPECT_CALL(*main, run()).WillOnce(testing::Throw(std::runtime_error("test error")));
    // error ログが出力されることを期待
    EXPECT_CALL(*logger_ptr, error(testing::HasSubstr("test error")));

    App app(std::move(main), std::move(human), std::move(bluetooth), std::move(buzzer), std::move(logger));
    int result = app.run();

    EXPECT_EQ(result, 1);
}

TEST(AppTest, Run_LogsAndReturns2OnUnknownException) {
    using namespace device_reminder;

    auto main = std::make_unique<StrictMock<MockMainTask>>();
    auto human = std::make_unique<StrictMock<MockHumanTask>>();
    auto bluetooth = std::make_unique<StrictMock<MockBluetoothTask>>();
    auto buzzer = std::make_unique<StrictMock<MockBuzzerTask>>();
    auto logger = std::make_unique<StrictMock<MockLogger>>();

    auto* logger_ptr = logger.get();

    // main_task の run が不明な例外を投げる
    EXPECT_CALL(*main, run()).WillOnce(testing::Throw(42));  // 整数例外
    // error ログが出力されることを期待
    EXPECT_CALL(*logger_ptr, error(testing::HasSubstr("Unknown exception")));

    App app(std::move(main), std::move(human), std::move(bluetooth), std::move(buzzer), std::move(logger));
    int result = app.run();

    EXPECT_EQ(result, 2);
}