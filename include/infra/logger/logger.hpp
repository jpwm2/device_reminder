#pragma once

#include "infra/logger/i_logger.hpp"
#include <spdlog/logger.h>
#include <memory>

namespace device_reminder {

/**
 * @brief spdlog を利用したロガー実装。
 *
 * メソッドの宣言順序は仕様書に従い
 * info → warn → error の順で記述している。
 * メンテナンス時もこの順序を保持すること。
 */
class Logger : public ILogger {
public:
    explicit Logger(std::shared_ptr<spdlog::logger> logger);

    /// 情報レベルのログを出力する（仕様順序の先頭）
    void info(const std::string& msg) override;
    /// 警告レベルのログを出力する（仕様順序の2番目）
    void warn(const std::string& msg) override;
    /// エラーレベルのログを出力する（仕様順序の最後）
    void error(const std::string& msg) override;

private:
    std::shared_ptr<spdlog::logger> logger_;
};

} // namespace device_reminder
