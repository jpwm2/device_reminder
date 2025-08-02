#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "infra/file_loader/file_loader.hpp"
#include "infra/gpio_operation/gpio_reader/gpio_reader.hpp"
#include "infra/logger/logger.hpp"
#include "infra/pir_driver/pir_driver.hpp"
#include "infra/thread_operation/thread_message/i_thread_message.hpp"
#include "infra/thread_operation/thread_message/thread_message_type.hpp"
#include "infra/thread_operation/thread_queue/i_thread_queue.hpp"
#include "infra/thread_operation/thread_sender/thread_sender.hpp"

#include "stubs/gpiod_stub.h"

#include <spdlog/logger.h>
#include <spdlog/sinks/base_sink.h>

#include <chrono>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

using namespace device_reminder;
using ::testing::_;
using ::testing::NiceMock;
using ::testing::StrictMock;

namespace {
class TestSink : public spdlog::sinks::base_sink<std::mutex> {
public:
  std::vector<std::string> messages;

protected:
  void sink_it_(const spdlog::details::log_msg &msg) override {
    spdlog::memory_buf_t buf;
    formatter_->format(msg, buf);
    messages.emplace_back(buf.data(), buf.size());
  }
  void flush_() override {}
};

class MockThreadQueue : public IThreadQueue {
public:
  MOCK_METHOD(void, push, (std::shared_ptr<IThreadMessage>), (override));
  MOCK_METHOD(std::shared_ptr<IThreadMessage>, pop, (), (override));
  MOCK_METHOD(size_t, size, (), (const, override));
};

class MockThreadMessage : public IThreadMessage {
public:
  MOCK_METHOD(ThreadMessageType, type, (), (const, noexcept, override));
  MOCK_METHOD(std::vector<std::string>, payload, (), (const, override));
  MOCK_METHOD(std::shared_ptr<IThreadMessage>, clone, (), (const, override));
  MOCK_METHOD(std::string, to_string, (), (const, override));
};
} // namespace

TEST(PIRDriverIntegrationTest,
     正常入力でメッセージが送信され情報ログが出力される) {
  gpiod_stub_reset();
  gpiod_stub_set_get_value_result(0);

  auto sink = std::make_shared<TestSink>();
  auto spd = std::make_shared<spdlog::logger>("test", sink);
  auto logger = std::make_shared<Logger>(spd);

  std::ofstream("/tmp/pir_config.txt").close();
  auto loader = std::make_shared<FileLoader>(logger, "/tmp/pir_config.txt");

  StrictMock<MockThreadQueue> queue;
  NiceMock<MockThreadMessage> message;
  auto sender = std::make_shared<ThreadSender>(
      logger, std::shared_ptr<IThreadQueue>(&queue, [](IThreadQueue *) {}),
      std::shared_ptr<IThreadMessage>(&message, [](IThreadMessage *) {}));

  auto gpio = std::make_shared<GPIOReader>(logger, 1);
  auto driver = std::make_unique<PIRDriver>(loader, logger, sender, gpio);

  EXPECT_CALL(queue, push(_)).Times(1);

  driver->run();
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  gpiod_stub_set_get_value_result(1);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  driver->stop();

  bool started_logged = false;
  for (const auto &text : sink->messages) {
    if (text.find("PIRDriver started") != std::string::npos) {
      started_logged = true;
      break;
    }
  }
  EXPECT_TRUE(started_logged);
}

TEST(PIRDriverIntegrationTest,
     異常入力でエラーログが出力されメッセージは送信されない) {
  gpiod_stub_reset();
  gpiod_stub_set_get_value_result(-1);

  auto sink = std::make_shared<TestSink>();
  auto spd = std::make_shared<spdlog::logger>("test", sink);
  auto logger = std::make_shared<Logger>(spd);

  std::ofstream("/tmp/pir_config.txt").close();
  auto loader = std::make_shared<FileLoader>(logger, "/tmp/pir_config.txt");

  StrictMock<MockThreadQueue> queue;
  NiceMock<MockThreadMessage> message;
  auto sender = std::make_shared<ThreadSender>(
      logger, std::shared_ptr<IThreadQueue>(&queue, [](IThreadQueue *) {}),
      std::shared_ptr<IThreadMessage>(&message, [](IThreadMessage *) {}));

  auto gpio = std::make_shared<GPIOReader>(logger, 1);
  auto driver = std::make_unique<PIRDriver>(loader, logger, sender, gpio);

  EXPECT_CALL(queue, push(_)).Times(0);

  driver->run();
  std::this_thread::sleep_for(std::chrono::milliseconds(30));
  driver->stop();

  bool error_logged = false;
  for (const auto &text : sink->messages) {
    if (text.find("PIRDriver read error") != std::string::npos) {
      error_logged = true;
      break;
    }
  }
  EXPECT_TRUE(error_logged);
}
