#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/process/process.hpp"
#include "infra/message/message_queue.hpp"
#include "infra/message/message_receiver.hpp"
#include "infra/message/message_dispatcher.hpp"
#include "infra/message/process_sender.hpp"
#include "infra/file_loader.hpp"
#include "infra/logger.hpp"

#include <thread>
#include <memory>

using ::testing::StrictMock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::InSequence;

using namespace device_reminder;

namespace device_reminder {

class MockReceiver : public IProcessReceiver {
public:
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(void, stop, (), (override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

class MockFileLoader : public IFileLoader {
public:
    MOCK_METHOD(int, load_int, (const std::string&), (const, override));
    MOCK_METHOD(std::string, load_string, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<std::string>, load_string_list, (const std::string&), (const, override));
};

class DummyQueue : public IProcessQueue {
public:
    void push(std::shared_ptr<IProcessMessage>) override {}
    std::shared_ptr<IProcessMessage> pop() override { return nullptr; }
    std::size_t size() const override { return 0; }
};

class DummyDispatcher : public IProcessDispatcher {
public:
    void dispatch(std::shared_ptr<IProcessMessage>) override {}
};

class DummySender : public IProcessSender {
public:
    void send() override {}
};

class TestProcessBase : public ProcessBase {
public:
    using ProcessBase::g_stop_flag;
    TestProcessBase(std::shared_ptr<IProcessQueue> queue,
                    std::shared_ptr<IProcessReceiver> receiver,
                    std::shared_ptr<IProcessDispatcher> dispatcher,
                    std::shared_ptr<IProcessSender> sender,
                    std::shared_ptr<IFileLoader> file_loader,
                    std::shared_ptr<ILogger> logger)
        : ProcessBase(std::move(queue), std::move(receiver), std::move(dispatcher),
                      std::move(sender), std::move(file_loader), std::move(logger),
                      "TestProcess") {}
};

} // namespace device_reminder

TEST(ProcessBaseTest, ConstructorLogsAndSetsPriority) {
    auto queue = std::make_shared<DummyQueue>();
    auto receiver = std::make_shared<MockReceiver>();
    auto dispatcher = std::make_shared<DummyDispatcher>();
    auto sender = std::make_shared<DummySender>();
    auto loader = std::make_shared<StrictMock<MockFileLoader>>();
    auto logger = std::make_shared<StrictMock<MockLogger>>();

    EXPECT_CALL(*loader, load_int("priority")).WillOnce(Return(3));
    EXPECT_CALL(*logger, info("ProcessBase initialized")).Times(1);

    TestProcessBase::g_stop_flag.store(false);
    TestProcessBase proc(queue, receiver, dispatcher, sender, loader, logger);

    EXPECT_EQ(proc.priority(), 3);
}

TEST(ProcessBaseTest, ConstructorNullLoggerDoesNotThrow) {
    auto loader = std::make_shared<NiceMock<MockFileLoader>>();
    EXPECT_CALL(*loader, load_int("priority")).WillOnce(Return(-1));

    TestProcessBase::g_stop_flag.store(false);
    EXPECT_NO_THROW({
        TestProcessBase proc(nullptr, nullptr, nullptr, nullptr, loader, nullptr);
        EXPECT_EQ(proc.priority(), -1);
    });
}

TEST(ProcessBaseTest, RunCallsReceiverAndLogs) {
    auto queue = std::make_shared<DummyQueue>();
    auto receiver = std::make_shared<StrictMock<MockReceiver>>();
    auto loader = std::make_shared<NiceMock<MockFileLoader>>();
    auto logger = std::make_shared<StrictMock<MockLogger>>();

    EXPECT_CALL(*loader, load_int("priority")).WillOnce(Return(1));
    EXPECT_CALL(*logger, info("ProcessBase initialized")).Times(1);
    {
        InSequence seq;
        EXPECT_CALL(*receiver, run()).Times(1);
        EXPECT_CALL(*logger, info("ProcessBase run start")).Times(1);
        EXPECT_CALL(*logger, info("ProcessBase stop requested")).Times(1);
        EXPECT_CALL(*receiver, stop()).Times(1);
        EXPECT_CALL(*logger, info("ProcessBase run end")).Times(1);
    }

    TestProcessBase::g_stop_flag.store(false);
    TestProcessBase proc(queue, receiver, nullptr, nullptr, loader, logger);

    std::thread th([&] { proc.run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    proc.stop();
    th.join();
}

TEST(ProcessBaseTest, StopSetsFlagAndLogs) {
    auto loader = std::make_shared<NiceMock<MockFileLoader>>();
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    EXPECT_CALL(*loader, load_int("priority")).WillOnce(Return(0));
    {
        InSequence s;
        EXPECT_CALL(*logger, info("ProcessBase initialized")).Times(1);
        EXPECT_CALL(*logger, info("ProcessBase stop requested")).Times(1);
    }

    TestProcessBase::g_stop_flag.store(false);
    TestProcessBase proc(nullptr, nullptr, nullptr, nullptr, loader, logger);

    proc.stop();
    EXPECT_TRUE(TestProcessBase::g_stop_flag.load());
}

