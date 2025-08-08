#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/process_operation/process_base/process_base.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/process_operation/process_receiver/i_process_receiver.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/watch_dog/i_watch_dog.hpp"

#include <memory>
#include <stdexcept>

using ::testing::StrictMock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::InSequence;
using ::testing::Throw;

using namespace device_reminder;

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

class MockWatchDog : public IWatchDog {
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(void, kick, (), (override));
};

class DummyQueue : public IProcessQueue {
public:
    void push(std::shared_ptr<IProcessMessage>) override {}
    std::shared_ptr<IProcessMessage> pop() override { return nullptr; }
    std::size_t size() const override { return 0; }
};

TEST(ProcessBaseTest, RunStartsComponents)
{
    auto receiver = std::make_shared<StrictMock<MockReceiver>>();
    auto queue = std::make_shared<DummyQueue>();
    auto loader = std::make_shared<StrictMock<MockFileLoader>>();
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    auto watchdog = std::make_shared<StrictMock<MockWatchDog>>();

    {
        InSequence s;
        EXPECT_CALL(*logger, info("ProcessBase run start"));
        EXPECT_CALL(*loader, load_int("priority")).WillOnce(Return(5));
        EXPECT_CALL(*watchdog, start());
        EXPECT_CALL(*receiver, run());
        EXPECT_CALL(*logger, info("ProcessBase run success"));
    }

    ProcessBase base(receiver, queue, loader, logger, watchdog);
    base.run();
}

TEST(ProcessBaseTest, StopStopsComponents)
{
    auto receiver = std::make_shared<StrictMock<MockReceiver>>();
    auto queue = std::make_shared<DummyQueue>();
    auto loader = std::make_shared<NiceMock<MockFileLoader>>();
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    auto watchdog = std::make_shared<StrictMock<MockWatchDog>>();

    {
        InSequence s;
        EXPECT_CALL(*logger, info("ProcessBase stop start"));
        EXPECT_CALL(*receiver, stop());
        EXPECT_CALL(*watchdog, stop());
        EXPECT_CALL(*logger, info("ProcessBase stop complete"));
    }

    ProcessBase base(receiver, queue, loader, logger, watchdog);
    base.stop();
}

TEST(ProcessBaseTest, RunErrorLogsAndThrows)
{
    auto receiver = std::make_shared<NiceMock<MockReceiver>>();
    auto queue = std::make_shared<DummyQueue>();
    auto loader = std::make_shared<StrictMock<MockFileLoader>>();
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    auto watchdog = std::make_shared<StrictMock<MockWatchDog>>();

    EXPECT_CALL(*logger, info("ProcessBase run start"));
    EXPECT_CALL(*loader, load_int("priority")).WillOnce(Return(1));
    EXPECT_CALL(*watchdog, start()).WillOnce(Throw(std::runtime_error("wd error")));
    EXPECT_CALL(*logger, error(testing::HasSubstr("wd error")));

    ProcessBase base(receiver, queue, loader, logger, watchdog);
    EXPECT_THROW(base.run(), std::runtime_error);
}

