#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/sinks/null_sink.h>
#include <fstream>
#include <unistd.h>
#include <cstdio>

#include "core/buzzer_task/buzzer_task.hpp"
#include "infra/buzzer_driver/buzzer_driver.hpp"
#include "infra/logger/logger.hpp"
#include "infra/file_loader/file_loader.hpp"
#include "infra/process_operation/process_sender/process_sender.hpp"
#include "infra/gpio_operation/gpio_setter/i_gpio_setter.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/process_operation/process_message/i_process_message.hpp"
#include "infra/process_operation/process_message/process_message_type.hpp"
#include "infra/thread_operation/thread_message/thread_message.hpp"

using namespace device_reminder;
using testing::NiceMock;
using testing::StrictMock;

namespace {

class MockGPIOSetter : public IGPIOSetter {
public:
    MOCK_METHOD(void, write, (bool value), (override));
};

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

class MockProcessQueue : public IProcessQueue {
public:
    MOCK_METHOD(void, push, (std::shared_ptr<IProcessMessage> msg), (override));
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

class BuzzerTaskIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        char name[] = "/tmp/buzzer_taskXXXXXX";
        int fd = mkstemp(name);
        close(fd);
        temp_file_path_ = name;
        std::ofstream ofs(temp_file_path_);
        ofs << "dummy=1";
        ofs.close();

        file_loader_logger_ = std::make_shared<NiceMock<MockLogger>>();
        file_loader_ = std::make_shared<FileLoader>(file_loader_logger_, temp_file_path_);

        queue_ = std::make_shared<NiceMock<MockProcessQueue>>();
        process_msg_ = std::make_shared<NiceMock<MockProcessMessage>>();
        sender_ = std::make_shared<ProcessSender>(queue_, process_msg_);

        auto sink = std::make_shared<spdlog::sinks::null_sink_mt>();
        auto spd = std::make_shared<spdlog::logger>("test", sink);
        logger_ = std::make_shared<Logger>(spd);

        gpio_ = std::make_shared<StrictMock<MockGPIOSetter>>();
        driver_loader_ = std::make_shared<NiceMock<MockFileLoader>>();
        driver_logger_ = std::make_shared<NiceMock<MockLogger>>();
        EXPECT_CALL(*driver_loader_, load_int("buzz_duration_ms")).WillOnce(testing::Return(100));
        driver_ = std::make_shared<BuzzerDriver>(driver_loader_, driver_logger_, gpio_);

        task_ = std::make_unique<BuzzerTask>(logger_, sender_, file_loader_, driver_);
    }

    void TearDown() override {
        std::remove(temp_file_path_.c_str());
    }

    std::string temp_file_path_;
    std::shared_ptr<NiceMock<MockLogger>> file_loader_logger_;
    std::shared_ptr<FileLoader> file_loader_;
    std::shared_ptr<NiceMock<MockProcessQueue>> queue_;
    std::shared_ptr<NiceMock<MockProcessMessage>> process_msg_;
    std::shared_ptr<ProcessSender> sender_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<StrictMock<MockGPIOSetter>> gpio_;
    std::shared_ptr<NiceMock<MockFileLoader>> driver_loader_;
    std::shared_ptr<NiceMock<MockLogger>> driver_logger_;
    std::shared_ptr<BuzzerDriver> driver_;
    std::unique_ptr<BuzzerTask> task_;
};

TEST_F(BuzzerTaskIntegrationTest, StartStopSequence) {
    testing::InSequence seq;
    EXPECT_CALL(*gpio_, write(true));
    EXPECT_CALL(*gpio_, write(false));

    ThreadMessage start_msg{ThreadMessageType::StartBuzzing, {}};
    task_->send_message(start_msg);
    EXPECT_EQ(task_->state(), BuzzerTask::State::Buzzing);

    ThreadMessage stop_msg{ThreadMessageType::StopBuzzing, {}};
    task_->send_message(stop_msg);
    EXPECT_EQ(task_->state(), BuzzerTask::State::WaitStart);
}

TEST_F(BuzzerTaskIntegrationTest, StopBeforeStartDoesNothing) {
    ThreadMessage stop_msg{ThreadMessageType::StopBuzzing, {}};
    task_->send_message(stop_msg);
    EXPECT_EQ(task_->state(), BuzzerTask::State::WaitStart);
}

TEST_F(BuzzerTaskIntegrationTest, StartTwiceOnlyCallsOnce) {
    EXPECT_CALL(*gpio_, write(true)).Times(1);

    ThreadMessage start_msg{ThreadMessageType::StartBuzzing, {}};
    task_->send_message(start_msg);
    task_->send_message(start_msg);

    EXPECT_EQ(task_->state(), BuzzerTask::State::Buzzing);
}

TEST_F(BuzzerTaskIntegrationTest, UnknownMessageIgnored) {
    ThreadMessage unknown{ThreadMessageType::None, {}};
    task_->send_message(unknown);
    EXPECT_EQ(task_->state(), BuzzerTask::State::WaitStart);
}

