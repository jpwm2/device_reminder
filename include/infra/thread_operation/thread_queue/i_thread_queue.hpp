#pragma once
// スレッド間通信で使用するキューのインタフェース
#include <cstddef>
#include <memory>

#include "infra/message/i_message.hpp"

namespace device_reminder {

class IThreadQueue {
public:
  virtual ~IThreadQueue() = default;

  // キューにメッセージを追加する
  virtual void push(std::shared_ptr<IMessage> msg) = 0;
  // キューからメッセージを取得する
  virtual std::shared_ptr<IMessage> pop() = 0;
  virtual size_t size() const = 0;
};

} // namespace device_reminder
