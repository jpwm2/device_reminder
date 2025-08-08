#pragma once
/**
 * @brief プロセスの抽象基底クラス
 *
 * 派生クラスは run() で実行開始し、stop() で終了処理を行う。
 */
class IProcessBase {
public:
    virtual ~IProcessBase() = default;

    /// プロセスの初期化と実行開始処理を行う
    virtual void run() = 0;

    /// プロセスの停止処理を行う
    virtual void stop() = 0;
};
