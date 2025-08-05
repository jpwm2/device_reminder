#pragma once

#include <string>

namespace device_reminder {

/**
 * @brief ログ出力のインターフェース。
 *
 * メソッドの宣言順序は仕様書に従い
 * info → warn → error の順で記述している。
 * 将来のメンテナンスでもこの順序を維持すること。
 */
class ILogger {
public:
    virtual ~ILogger() = default;

    /// 情報レベルのログを出力する（仕様順序の先頭）
    virtual void info(const std::string& msg) = 0;
    /// 警告レベルのログを出力する（仕様順序の2番目）
    virtual void warn(const std::string& msg) = 0;
    /// エラーレベルのログを出力する（仕様順序の最後）
    virtual void error(const std::string& msg) = 0;
};

} // namespace device_reminder
