#pragma once

namespace device_reminder {

class IGPIOReader {
public:
    virtual ~IGPIOReader() = default;
    /// 現在の入力状態を読み取る (true: HIGH, false: LOW)
    virtual bool read() = 0;
    /// 指定したエッジが検出されるまでブロッキングで待機する
    virtual void poll_edge(bool detect_rising) = 0;
};

} // namespace device_reminder

