#pragma once

#include <string>
#include <vector>

namespace device_reminder {

/**
 * @brief 設定ファイルから値を取得するためのインタフェース。
 *
 * 事前に指定されたキーに対応する値を読み出す API を提供する。
 * 各呼び出しでは設定ファイルへアクセスを行うため、高頻度で呼び出す
 * 場合はパフォーマンスに注意すること。
 *
 * エラー発生時には実装側で例外を送出する。スレッドセーフ性は保証されず、
 * 複数スレッドから同時に利用する場合は呼び出し側で同期を行うこと。
 */
class IFileLoader {
public:
  virtual ~IFileLoader() = default;

  /**
   * @brief 値を整数として取得する。
   * @return 読み込まれた整数値。
   * @throws std::runtime_error
   * ファイルアクセスやキー未存在などの致命的な問題が発生した場合。
   * @throws std::invalid_argument, std::out_of_range
   * 整数への変換に失敗した場合。
   */
  virtual int load_int() const = 0;

  /**
   * @brief 値を文字列として取得する。
   * @return 読み込まれた文字列。
   * @throws std::runtime_error
   * ファイルアクセスやキー未存在などの致命的な問題が発生した場合。
   */
  virtual std::string load_string() const = 0;

  /**
   * @brief 値をカンマ区切りの文字列リストとして取得する。
   * @return 取り出した文字列のリスト。
   * @throws std::runtime_error
   * ファイルアクセスやキー未存在などの致命的な問題が発生した場合。
   */
  virtual std::vector<std::string> load_string_list() const = 0;
};

} // namespace device_reminder
