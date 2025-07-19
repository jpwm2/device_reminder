#pragma once
#include "message/i_message.hpp"
#include <cstddef>   // size_t

namespace device_reminder {

//--------------------------------------
// 具象クラス: Message
//--------------------------------------
struct Message final : public IMessage {
    // コンストラクタ
    constexpr Message(MessageType t = MessageType::None,
                      bool        p = false) noexcept
        : type_{t}, payload_{p} {}

    // ------ IMessage 実装 ------
    MessageType type() const noexcept override   { return type_; }
    bool        payload() const noexcept override{ return payload_; }

    //----------------------------------
    // データメンバー
    //----------------------------------
    MessageType type_;
    bool        payload_;
} __attribute__((packed));    // サイズ固定化（GCC/Clang系）

// 固定サイズを外部でも利用できるよう公開
constexpr std::size_t MESSAGE_SIZE = sizeof(Message);

static_assert(MESSAGE_SIZE == sizeof(Message),
              "Unexpected Message size");

} // namespace device_reminder
