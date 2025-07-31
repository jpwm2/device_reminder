#pragma once
#include <memory>

#include "thread_message_operation/i_message_queue.hpp"

/// 事前宣言 ― 依存インターフェース
class IThreadMessageQueue;

/**
 * @brief プロセスの抽象基底クラス  
 * 内部でスレッドを起動し `run()` がブロッキングして返るまで
 * プロセスが継続するという最小契約だけを定義する。
 */
class IProcessBase {
public:
    virtual ~IProcessBase() = default;

    /**
     * @return 正常終了コード（通常は 0）
     *
     * - 実装は内部スレッドを起動して待機し、
     *   終了要求を受け取ったら join して戻る。
     */
    virtual int run() = 0;

    /// 非同期に終了要求を出す（Ctrl‑C 以外でも停止できるように）
    virtual void stop() = 0;

    /// 優先度を取得（値が小さいほど高優先）
    virtual int priority() const noexcept = 0;
};
