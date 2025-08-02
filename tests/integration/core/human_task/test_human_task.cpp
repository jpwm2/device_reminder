#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <spdlog/spdlog.h>

#include "core/human_task/human_task.hpp"
#include "infra/logger/logger.hpp"
#include "infra/pir_driver/pir_driver.hpp"
#include "infra/process_operation/process_sender/process_sender.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/thread_operation/thread_sender/i_thread_sender.hpp"
#include "infra/gpio_operation/gpio_reader/i_gpio_reader.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/process_operation/process_message/i_process_message.hpp"

using namespace device_reminder;
using ::testing::StrictMock;
using ::testing::Return;

namespace {

class MockFileLoader : public IFileLoader {
public:
    MOCK_METHOD(int, load_int, (const std::string& key), (const, override));
    MOCK_METHOD(std::string, load_string, (const std::string& key), (const, override));
    MOCK_METHOD(std::vector<std::string>, load_string_list, (const std::string& key), (const, override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string& msg), (override));
    MOCK_METHOD(void, error, (const std::string& msg), (override));
    MOCK_METHOD(void, warn, (const std::string& msg), (override));
};

class MockThreadSender : public IThreadSender {
public:
    MOCK_METHOD(void, send, (), (override));
};

class MockGPIOReader : public IGPIOReader {
public:
    MOCK_METHOD(bool, read, (), (override));
};

class MockProcessQueue : public IProcessQueue {
public:
    MOCK_METHOD(void, push, (std::shared_ptr<IProcessMessage>), (override));
    MOCK_METHOD(std::shared_ptr<IProcessMessage>, pop, (), (override));
    MOCK_METHOD(std::size_t, size, (), (const, override));
};

class MockProcessMessage : public IProcessMessage {
public:
    MOCK_METHOD(ProcessMessageType, type, (), (const, noexcept, override));
    MOCK_METHOD(std::vector<std::string>, payload, (), (const, noexcept, override));
    MOCK_METHOD(std::shared_ptr<IProcessMessage>, clone, (), (const, override));
    MOCK_METHOD(std::string, to_string, (), (const, override));
};

} // namespace

TEST(HumanTaskIntegrationTest, NormalDetectionCallsSendOnce) {
    StrictMock<MockFileLoader> loader;
    StrictMock<MockLogger> pir_logger;
    StrictMock<MockThreadSender> thread_sender;
    StrictMock<MockGPIOReader> gpio;
    StrictMock<MockProcessQueue> process_queue;
    StrictMock<MockProcessMessage> process_message;

    auto spd = spdlog::default_logger()->clone("human_task_normal");
    auto logger = std::make_shared<Logger>(spd);
    auto process_sender = std::make_shared<ProcessSender>(
        std::shared_ptr<IProcessQueue>(&process_queue, [](IProcessQueue*){}),
        std::shared_ptr<IProcessMessage>(&process_message, [](IProcessMessage*){}));
    auto pir = std::make_shared<PIRDriver>(
        std::shared_ptr<IFileLoader>(&loader, [](IFileLoader*){}),
        std::shared_ptr<ILogger>(&pir_logger, [](ILogger*){}),
        std::shared_ptr<IThreadSender>(&thread_sender, [](IThreadSender*){}),
        std::shared_ptr<IGPIOReader>(&gpio, [](IGPIOReader*){}));

    EXPECT_CALL(pir_logger, info(testing::_)).Times(testing::AnyNumber());
    {
        testing::InSequence seq;
        EXPECT_CALL(gpio, read()).WillOnce(Return(false));
        EXPECT_CALL(gpio, read()).WillOnce(Return(true));
        EXPECT_CALL(gpio, read()).WillRepeatedly(Return(true));
    }
    EXPECT_CALL(thread_sender, send()).Times(1);

    HumanTask task(logger, pir, process_sender);

    task.on_detecting({});
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    task.on_stopping({});
}

TEST(HumanTaskIntegrationTest, AbnormalDetectionLogsErrorAndNoSend) {
    StrictMock<MockFileLoader> loader;
    StrictMock<MockLogger> pir_logger;
    StrictMock<MockThreadSender> thread_sender;
    StrictMock<MockGPIOReader> gpio;
    StrictMock<MockProcessQueue> process_queue;
    StrictMock<MockProcessMessage> process_message;

    auto spd = spdlog::default_logger()->clone("human_task_abnormal");
    auto logger = std::make_shared<Logger>(spd);
    auto process_sender = std::make_shared<ProcessSender>(
        std::shared_ptr<IProcessQueue>(&process_queue, [](IProcessQueue*){}),
        std::shared_ptr<IProcessMessage>(&process_message, [](IProcessMessage*){}));
    auto pir = std::make_shared<PIRDriver>(
        std::shared_ptr<IFileLoader>(&loader, [](IFileLoader*){}),
        std::shared_ptr<ILogger>(&pir_logger, [](ILogger*){}),
        std::shared_ptr<IThreadSender>(&thread_sender, [](IThreadSender*){}),
        std::shared_ptr<IGPIOReader>(&gpio, [](IGPIOReader*){}));

    EXPECT_CALL(pir_logger, info(testing::_)).Times(testing::AnyNumber());
    EXPECT_CALL(pir_logger, error(testing::_)).Times(1);
    EXPECT_CALL(gpio, read()).WillOnce(testing::Throw(std::runtime_error("read fail")));
    EXPECT_CALL(thread_sender, send()).Times(0);

    HumanTask task(logger, pir, process_sender);

    task.on_detecting({});
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    task.on_stopping({});
}

