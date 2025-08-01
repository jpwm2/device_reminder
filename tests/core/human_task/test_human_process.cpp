#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "human_task/human_process.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/watch_dog/i_watch_dog.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/process_operation/process_message/i_process_message.hpp"
#include "infra/process_operation/process_receiver/i_process_receiver.hpp"
#include "infra/process_operation/process_dispatcher/i_process_dispatcher.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "core/interfaces/i_handler.hpp"
#include "core/human_task/i_human_task.hpp"
#include <thread>
#include <chrono>

using ::testing::StrictMock;
using ::testing::NiceMock;
using ::testing::Return;

namespace device_reminder {

class DummyQueue : public IProcessQueue {
public:
    void push(std::shared_ptr<IProcessMessage>) override {}
    std::shared_ptr<IProcessMessage> pop() override { return nullptr; }
    std::size_t size() const override { return 0; }
};

class MockReceiver : public IProcessReceiver {
public:
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};

class MockDispatcher : public IProcessDispatcher {
public:
    MOCK_METHOD(void, dispatch, (std::shared_ptr<IProcessMessage>), (override));
};

class MockSender : public IProcessSender {
public:
    MOCK_METHOD(void, send, (), (override));
};

class MockLoader : public IFileLoader {
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

class MockHandler : public IHandler {
public:
    MOCK_METHOD(void, handle, (std::shared_ptr<IProcessMessage>), (override));
};

class MockTask : public IHumanTask {
public:
    MOCK_METHOD(void, on_detecting, (const std::vector<std::string>&), (override));
    MOCK_METHOD(void, on_stopping, (const std::vector<std::string>&), (override));
    MOCK_METHOD(void, on_cooldown, (const std::vector<std::string>&), (override));
};

TEST(HumanProcessTest, ConstructLogsWhenLoggerProvided) {
    auto queue = std::make_shared<DummyQueue>();
    auto receiver = std::make_shared<StrictMock<MockReceiver>>();
    auto dispatcher = std::make_shared<StrictMock<MockDispatcher>>();
    auto sender = std::make_shared<StrictMock<MockSender>>();
    auto loader = std::make_shared<StrictMock<MockLoader>>();
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    auto watchdog = std::make_shared<StrictMock<MockWatchDog>>();
    auto handler = std::make_shared<StrictMock<MockHandler>>();
    auto task = std::make_shared<StrictMock<MockTask>>();

    EXPECT_CALL(*loader, load_int("priority")).WillOnce(Return(0));

    EXPECT_CALL(*loader, load_int("priority")).WillOnce(Return(0));
    EXPECT_CALL(*logger, info("ProcessBase initialized")).Times(1);
    EXPECT_CALL(*logger, info("HumanProcess created")).Times(1);
    HumanProcess proc(queue, receiver, dispatcher, sender, loader, logger, watchdog, handler, task);
}

TEST(HumanProcessTest, ConstructNoLogger) {
    auto queue = std::make_shared<DummyQueue>();
    auto receiver = std::make_shared<StrictMock<MockReceiver>>();
    auto dispatcher = std::make_shared<StrictMock<MockDispatcher>>();
    auto sender = std::make_shared<StrictMock<MockSender>>();
    auto loader = std::make_shared<StrictMock<MockLoader>>();
    auto watchdog = std::make_shared<StrictMock<MockWatchDog>>();
    auto handler = std::make_shared<StrictMock<MockHandler>>();
    auto task = std::make_shared<StrictMock<MockTask>>();

    HumanProcess proc(queue, receiver, dispatcher, sender, loader, nullptr, watchdog, handler, task);
}

TEST(HumanProcessTest, RunStartsAndStopsWatchDog) {
    auto queue = std::make_shared<DummyQueue>();
    auto receiver = std::make_shared<NiceMock<MockReceiver>>();
    auto dispatcher = std::make_shared<NiceMock<MockDispatcher>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto loader = std::make_shared<NiceMock<MockLoader>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    auto watchdog = std::make_shared<StrictMock<MockWatchDog>>();
    auto handler = std::make_shared<NiceMock<MockHandler>>();
    auto task = std::make_shared<NiceMock<MockTask>>();

    HumanProcess proc(queue, receiver, dispatcher, sender, loader, logger, watchdog, handler, task);
    EXPECT_CALL(*watchdog, start()).Times(1);
    EXPECT_CALL(*receiver, run()).Times(1);
    EXPECT_CALL(*receiver, stop()).Times(1);
    EXPECT_CALL(*watchdog, stop()).Times(1);
    std::thread th([&]{
        proc.run();
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    proc.stop();
    th.join();
}

TEST(HumanProcessTest, RunWithoutWatchDog) {
    auto queue = std::make_shared<DummyQueue>();
    auto receiver = std::make_shared<NiceMock<MockReceiver>>();
    auto dispatcher = std::make_shared<NiceMock<MockDispatcher>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto loader = std::make_shared<NiceMock<MockLoader>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    auto handler = std::make_shared<NiceMock<MockHandler>>();
    auto task = std::make_shared<NiceMock<MockTask>>();

    HumanProcess proc(queue, receiver, dispatcher, sender, loader, logger, nullptr, handler, task);
    EXPECT_CALL(*receiver, run()).Times(1);
    EXPECT_CALL(*receiver, stop()).Times(1);
    std::thread th([&]{ proc.run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    proc.stop();
    th.join();
}

TEST(HumanProcessTest, StopCallsWatchDog) {
    auto queue = std::make_shared<DummyQueue>();
    auto receiver = std::make_shared<NiceMock<MockReceiver>>();
    auto dispatcher = std::make_shared<NiceMock<MockDispatcher>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto loader = std::make_shared<NiceMock<MockLoader>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    auto watchdog = std::make_shared<StrictMock<MockWatchDog>>();
    auto handler = std::make_shared<NiceMock<MockHandler>>();
    auto task = std::make_shared<NiceMock<MockTask>>();

    HumanProcess proc(queue, receiver, dispatcher, sender, loader, logger, watchdog, handler, task);
    EXPECT_CALL(*watchdog, stop()).Times(1);
    proc.stop();
}

TEST(HumanProcessTest, StopWithoutWatchDog) {
    auto queue = std::make_shared<DummyQueue>();
    auto receiver = std::make_shared<NiceMock<MockReceiver>>();
    auto dispatcher = std::make_shared<NiceMock<MockDispatcher>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto loader = std::make_shared<NiceMock<MockLoader>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();
    auto handler = std::make_shared<NiceMock<MockHandler>>();
    auto task = std::make_shared<NiceMock<MockTask>>();

    HumanProcess proc(queue, receiver, dispatcher, sender, loader, logger, nullptr, handler, task);
    EXPECT_NO_THROW(proc.stop());
}

} // namespace device_reminder
