#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/watch_dog/i_watch_dog.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/process_operation/process_receiver/i_process_receiver.hpp"
#include "infra/process_operation/process_dispatcher/i_process_dispatcher.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include "core/interfaces/i_handler.hpp"
#include "core/bluetooth_task/i_bluetooth_task.hpp"

#include <thread>
#include <memory>

using ::testing::NiceMock;
using ::testing::StrictMock;
using ::testing::Return;
using ::testing::_;

#include "bluetooth_task/bluetooth_process.hpp"
using namespace device_reminder;

namespace device_reminder {

class MockFileLoader : public IFileLoader {
public:
    MOCK_METHOD(int, load_int, (const std::string&), (const, override));
    MOCK_METHOD(std::string, load_string, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<std::string>, load_string_list, (const std::string&), (const, override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

class MockWatchDog : public IWatchDog {
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(void, kick, (), (override));
};

class DummyHandler : public IHandler {
public:
    void handle(std::shared_ptr<IProcessMessage>) override {}
};

class DummyTask : public IBluetoothTask {
public:
    void on_waiting(const std::vector<std::string>&) override {}
};

class DummyQueue : public IProcessQueue {
public:
    void push(std::shared_ptr<IProcessMessage>) override {}
    std::shared_ptr<IProcessMessage> pop() override { return nullptr; }
    std::size_t size() const override { return 0; }
};

class DummyReceiver : public IProcessReceiver {
public:
    void run() override {}
    void stop() override {}
};

class DummyDispatcher : public IProcessDispatcher {
public:
    void dispatch(std::shared_ptr<IProcessMessage>) override {}
};

class DummySender : public IProcessSender {
public:
    void send() override {}
};

TEST(BluetoothProcessTest, ConstructorLogsAndSetsPriority) {
    auto queue = std::make_shared<DummyQueue>();
    auto receiver = std::make_shared<DummyReceiver>();
    auto dispatcher = std::make_shared<DummyDispatcher>();
    auto sender = std::make_shared<DummySender>();
    auto file_loader = std::make_shared<StrictMock<MockFileLoader>>();
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    auto watch_dog = std::make_shared<NiceMock<MockWatchDog>>();
    auto handler = std::make_shared<DummyHandler>();
    auto task = std::make_shared<DummyTask>();

    EXPECT_CALL(*file_loader, load_int("priority")).WillOnce(Return(5));
    EXPECT_CALL(*logger, info("BluetoothProcess created")).Times(1);

    BluetoothProcess proc(queue, receiver, dispatcher, sender, file_loader, logger,
                          watch_dog, handler, task);

    EXPECT_EQ(proc.priority(), 5);
}

TEST(BluetoothProcessTest, ConstructorNullLoggerDoesNotThrow) {
    auto file_loader = std::make_shared<NiceMock<MockFileLoader>>();
    EXPECT_CALL(*file_loader, load_int("priority")).WillOnce(Return(-1));

    EXPECT_NO_THROW({
        BluetoothProcess proc(nullptr, nullptr, nullptr, nullptr, file_loader,
                              nullptr, nullptr, nullptr, nullptr);
        EXPECT_EQ(proc.priority(), 0);
    });
}

TEST(BluetoothProcessTest, RunStartsAndStopsWatchDog) {
    auto queue = std::make_shared<DummyQueue>();
    auto receiver = std::make_shared<DummyReceiver>();
    auto dispatcher = std::make_shared<DummyDispatcher>();
    auto sender = std::make_shared<DummySender>();
    auto file_loader = std::make_shared<NiceMock<MockFileLoader>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    auto watch_dog = std::make_shared<StrictMock<MockWatchDog>>();
    auto handler = std::make_shared<DummyHandler>();
    auto task = std::make_shared<DummyTask>();

    BluetoothProcess proc(queue, receiver, dispatcher, sender, file_loader, logger,
                          watch_dog, handler, task);

    {
        ::testing::InSequence seq;
        EXPECT_CALL(*watch_dog, start()).Times(1);
        EXPECT_CALL(*watch_dog, stop()).Times(1); // called from stop()
        EXPECT_CALL(*watch_dog, stop()).Times(1); // called from run()
    }

    std::thread th([&] { proc.run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    proc.stop();
    th.join();
}

TEST(BluetoothProcessTest, StopCallsWatchDogStop) {
    auto watch_dog = std::make_shared<StrictMock<MockWatchDog>>();

    BluetoothProcess proc(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                          watch_dog, nullptr, nullptr);

    EXPECT_CALL(*watch_dog, stop()).Times(1);
    proc.stop();
}

TEST(BluetoothProcessTest, StopWithNullWatchDogDoesNothing) {
    BluetoothProcess proc(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                          nullptr, nullptr, nullptr);
    EXPECT_NO_THROW(proc.stop());
}

} // namespace device_reminder

