//
// 統合テスト【process_receiver】
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "infra/process_operation/process_dispatcher/process_dispatcher.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include "infra/process_operation/process_queue/process_queue.hpp"
#include "infra/process_operation/process_receiver/process_receiver.hpp"
#include "posix_mq_stub.h"

#include <future>
#include <thread>

using namespace device_reminder;
using ::testing::_;
using ::testing::Return;

namespace {

// ILogger の空実装
class TestLogger : public ILogger {
public:
  void info(const std::string &) override {}
  void error(const std::string &) override {}
  void warn(const std::string &) override {}
};

// IMessageCodec のモック
class MockMessageCodec : public IMessageCodec {
public:
  MOCK_METHOD(std::vector<uint8_t>, encode, (std::shared_ptr<IProcessMessage>),
              (override));
  MOCK_METHOD(std::shared_ptr<IProcessMessage>, decode,
              (const std::vector<uint8_t> &), (override));
};

} // namespace

// 正常系: メッセージがディスパッチされることを確認
TEST(ProcessReceiverIntegrationTest, DispatchesMessage) {
  mq_stub_reset();

  auto logger = std::make_shared<TestLogger>();
  auto codec = std::make_shared<MockMessageCodec>();
  auto queue = std::make_shared<ProcessQueue>(logger, codec, "/test_queue");

  auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing,
                                              std::vector<std::string>{});

  EXPECT_CALL(*codec, encode(_))
      .WillOnce(Return(std::vector<uint8_t>{0x01, 0x02}));
  EXPECT_CALL(*codec, decode(_))
      .Times(testing::AtLeast(1))
      .WillOnce(Return(msg))
      .WillRepeatedly(Return(std::shared_ptr<IProcessMessage>{}));

  testing::MockFunction<void(std::shared_ptr<IProcessMessage>)> handler;

  std::promise<void> handler_called;
  std::promise<void> handler_release;

  auto msg_base = std::static_pointer_cast<IProcessMessage>(msg);
  EXPECT_CALL(handler, Call(msg_base))
      .WillOnce([&](std::shared_ptr<IProcessMessage>) {
        handler_called.set_value();
        handler_release.get_future().wait();
      });

  ProcessDispatcher::HandlerMap map{
      {ProcessMessageType::StartBuzzing, handler.AsStdFunction()}};
  auto dispatcher = std::make_shared<ProcessDispatcher>(logger, map);

  ProcessReceiver receiver(logger, queue, dispatcher);

  queue->push(msg);
  receiver.run();

  handler_called.get_future().wait();
  handler_release.set_value();
  receiver.stop();
}

// 異常系: デコード失敗時にハンドラが呼ばれないことを確認
TEST(ProcessReceiverIntegrationTest, DecodeFailure) {
  mq_stub_reset();

  auto logger = std::make_shared<TestLogger>();
  auto codec = std::make_shared<MockMessageCodec>();
  auto queue = std::make_shared<ProcessQueue>(logger, codec, "/test_queue");

  auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing,
                                              std::vector<std::string>{});

  EXPECT_CALL(*codec, encode(_))
      .WillOnce(Return(std::vector<uint8_t>{0x01, 0x02}));
  testing::MockFunction<void(std::shared_ptr<IProcessMessage>)> handler;
  EXPECT_CALL(handler, Call(_)).Times(0);

  std::promise<void> decode_called;
  std::promise<void> decode_release;

  EXPECT_CALL(*codec, decode(_))
      .Times(testing::AtLeast(1))
      .WillOnce([&](const std::vector<uint8_t> &) {
        decode_called.set_value();
        decode_release.get_future().wait();
        return std::shared_ptr<IProcessMessage>{};
      })
      .WillRepeatedly(Return(std::shared_ptr<IProcessMessage>{}));

  ProcessDispatcher::HandlerMap map{
      {ProcessMessageType::StartBuzzing, handler.AsStdFunction()}};
  auto dispatcher = std::make_shared<ProcessDispatcher>(logger, map);

  ProcessReceiver receiver(logger, queue, dispatcher);

  queue->push(msg);
  receiver.run();

  decode_called.get_future().wait();
  decode_release.set_value();
  receiver.stop();
}
