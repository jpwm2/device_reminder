#pragma once

#include "infra/file_loader/i_file_loader.hpp"
#include "infra/logger/i_logger.hpp"

#include <memory>
#include <string>

namespace device_reminder {

/**
 * @brief ファイルから設定値を読み出す `IFileLoader` の実装。
 *
 * 各読み出しメソッドは呼び出しごとにファイルへアクセスし、コンストラクタで
 * 指定されたキーに対応する値を取得する。エラー発生時には `ILogger` を通して
 * 記録し、`std::runtime_error` を送出する。
 *
 * スレッドセーフ性は保証されないため、同一インスタンスを複数スレッドから
 * 利用する場合は外部で同期すること。
 */
class FileLoader : public IFileLoader {
public:
  /**
   * @brief ファイルパスとキーを指定してローダを構築する。
   *
   * @param logger ログ出力に使用する `ILogger`。`nullptr`
   * の場合はログ出力しない。
   * @param file_path 読み込む設定ファイルのパス。
   * @param key 取得対象となるキー。コンストラクタで保持される。
   */
  FileLoader(std::shared_ptr<ILogger> logger, const std::string &file_path,
             const std::string &key);

  /// @copydoc IFileLoader::load_int
  int load_int() const override;

  /// @copydoc IFileLoader::load_string
  std::string load_string() const override;

  /// @copydoc IFileLoader::load_string_list
  std::vector<std::string> load_string_list() const override;

private:
  /**
   * @brief `key_` に対応する文字列値をファイルから取得する。
   * @return 取得した文字列。
   * @throws std::runtime_error ファイルオープン失敗やキー未存在の場合。
   */
  std::string load_value() const;

  std::shared_ptr<ILogger> logger_; ///< ログ出力に使用するロガー。
  std::string file_path_;           ///< 対象となる設定ファイルのパス。
  std::string key_;                 ///< 取得対象のキー。
};

} // namespace device_reminder
