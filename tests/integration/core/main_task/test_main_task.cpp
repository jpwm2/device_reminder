#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "core/main_task/main_task.hpp"
#include "infra/file_loader/file_loader.hpp"
#include "infra/logger/logger.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/process_operation/process_sender/process_sender.hpp"
#include "infra/thread_operation/thread_message/thread_message.hpp"
#include "infra/thread_operation/thread_sender/i_thread_sender.hpp"
#include "infra/timer_service/timer_service.hpp"

#include <spdlog/logger.h>
#include <spdlog/sinks/null_sink.h>

#include <chrono>
#include <cstdio>
#include <fstream>
#include <thread>

using ::testing::_;
using ::testing::NiceMock;
using ::testing::StrictMock;

namespace device_reminder {

class MockProcessQueue : public IProcessQueue {
public:
  MOCK_METHOD(void, push, (std::shared_ptr<IProcessMessage>), (override));
  MOCK_METHOD(std::shared_ptr<IProcessMessage>, pop, (), (override));
  MOCK_METHOD(std::size_t, size, (), (const, override));
};

class MockThreadSender : public IThreadSender {
public:
  MOCK_METHOD(void, send, (), (override));
};

std::shared_ptr<Logger> create_logger() {
  auto sink = std::make_shared<spdlog::sinks::null_sink_mt>();
  auto spdlogger = std::make_shared<spdlog::logger>("test", sink);
  return std::make_shared<Logger>(spdlogger);
}

std::shared_ptr<FileLoader>
create_file_loader(const std::shared_ptr<Logger> &logger,
                   const std::string &path) {
  std::ofstream ofs(path);
  ofs << "device_list=phone\n";
  ofs.close();
  return std::make_shared<FileLoader>(logger, path);
}

std::shared_ptr<ProcessSender>
create_sender(const std::shared_ptr<IProcessQueue> &queue,
              ProcessMessageType type) {
  auto msg = std::make_shared<ProcessMessage>(type, std::vector<std::string>{});
  return std::make_shared<ProcessSender>(queue, msg);
}

TEST(MainTaskIntegrationTest, HumanDetectedStartsBluetoothAndTimer) {
  auto logger = create_logger();
  const std::string file_path = "test_devices_normal.txt";
  auto file_loader = create_file_loader(logger, file_path);

  auto human_queue = std::make_shared<NiceMock<MockProcessQueue>>();
  auto human_start =
      create_sender(human_queue, ProcessMessageType::StartHumanDetection);
  auto human_stop =
      create_sender(human_queue, ProcessMessageType::StopHumanDetection);

  auto bt_queue = std::make_shared<StrictMock<MockProcessQueue>>();
  auto bluetooth =
      create_sender(bt_queue, ProcessMessageType::RequestBluetoothScan);

  auto buz_queue = std::make_shared<NiceMock<MockProcessQueue>>();
  auto buz_start = create_sender(buz_queue, ProcessMessageType::StartBuzzing);
  auto buz_stop = create_sender(buz_queue, ProcessMessageType::StopBuzzing);

  auto det_thread_sender = std::make_shared<StrictMock<MockThreadSender>>();
  auto det_timer = std::make_shared<TimerService>(logger, 0, det_thread_sender);

  auto cd_thread_sender = std::make_shared<NiceMock<MockThreadSender>>();
  auto cooldown_timer =
      std::make_shared<TimerService>(logger, 0, cd_thread_sender);

  MainTask task(logger, file_loader, human_start, human_stop, bluetooth,
                buz_start, buz_stop, det_timer, cooldown_timer);

  EXPECT_CALL(*bt_queue, push(_));
  EXPECT_CALL(*det_thread_sender, send());

  task.run(ThreadMessage{ThreadMessageType::HumanDetected, {}});
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  EXPECT_EQ(task.state(), MainTask::State::WaitDeviceResponse);

  std::remove(file_path.c_str());
}

TEST(MainTaskIntegrationTest, UnknownDeviceTriggersBuzzerAndCooldown) {
  auto logger = create_logger();
  const std::string file_path = "test_devices_abnormal.txt";
  auto file_loader = create_file_loader(logger, file_path);

  auto human_queue = std::make_shared<NiceMock<MockProcessQueue>>();
  auto human_start =
      create_sender(human_queue, ProcessMessageType::StartHumanDetection);
  auto human_stop =
      create_sender(human_queue, ProcessMessageType::StopHumanDetection);

  auto bt_queue = std::make_shared<NiceMock<MockProcessQueue>>();
  auto bluetooth =
      create_sender(bt_queue, ProcessMessageType::RequestBluetoothScan);

  auto buz_queue = std::make_shared<StrictMock<MockProcessQueue>>();
  auto buz_start = create_sender(buz_queue, ProcessMessageType::StartBuzzing);
  auto buz_stop = create_sender(std::make_shared<NiceMock<MockProcessQueue>>(),
                                ProcessMessageType::StopBuzzing);

  auto det_thread_sender = std::make_shared<NiceMock<MockThreadSender>>();
  auto det_timer = std::make_shared<TimerService>(logger, 0, det_thread_sender);

  auto cd_thread_sender = std::make_shared<StrictMock<MockThreadSender>>();
  auto cooldown_timer =
      std::make_shared<TimerService>(logger, 0, cd_thread_sender);

  MainTask task(logger, file_loader, human_start, human_stop, bluetooth,
                buz_start, buz_stop, det_timer, cooldown_timer);

  task.run(ThreadMessage{ThreadMessageType::HumanDetected, {}});
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  EXPECT_CALL(*buz_queue, push(_));
  EXPECT_CALL(*cd_thread_sender, send());

  task.run(ThreadMessage{ThreadMessageType::RespondDeviceFound, {"unknown"}});
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  EXPECT_EQ(task.state(), MainTask::State::ScanCooldown);

  std::remove(file_path.c_str());
}

} // namespace device_reminder
