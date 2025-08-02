// NOLINTNEXTLINE: integration test
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <spdlog/sinks/null_sink.h>

#include "infra/gpio_operation/gpio_reader/gpio_reader.hpp"
#include "infra/logger/logger.hpp"

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::StrEq;

// -----------------------------------------------------------------------------
// gpiod モックの定義
// -----------------------------------------------------------------------------

// gpiod ラッパーモック
class MockGpiod {
public:
  MOCK_METHOD(struct gpiod_chip *, chip_open, (const char *));
  MOCK_METHOD(void, chip_close, (struct gpiod_chip *));
  MOCK_METHOD(struct gpiod_line *, chip_get_line,
              (struct gpiod_chip *, unsigned int));
  MOCK_METHOD(int, line_request_input, (struct gpiod_line *, const char *));
  MOCK_METHOD(int, line_get_value, (struct gpiod_line *));
  MOCK_METHOD(void, line_release, (struct gpiod_line *));
};

// グローバルモックへのポインタ
static MockGpiod *g_gpiod_mock = nullptr;

// gpiod.h の前方宣言に対応した擬似的な構造体定義
struct gpiod_chip {
  int dummy;
};
struct gpiod_line {
  int dummy;
};

extern "C" {
// C API をモックに委譲
struct gpiod_chip *gpiod_chip_open(const char *chipname) {
  return g_gpiod_mock->chip_open(chipname);
}

void gpiod_chip_close(struct gpiod_chip *chip) {
  g_gpiod_mock->chip_close(chip);
}

struct gpiod_line *gpiod_chip_get_line(struct gpiod_chip *chip,
                                       unsigned int line) {
  return g_gpiod_mock->chip_get_line(chip, line);
}

int gpiod_line_request_input(struct gpiod_line *line, const char *consumer) {
  return g_gpiod_mock->line_request_input(line, consumer);
}

int gpiod_line_get_value(struct gpiod_line *line) {
  return g_gpiod_mock->line_get_value(line);
}

void gpiod_line_release(struct gpiod_line *line) {
  g_gpiod_mock->line_release(line);
}

// 他モジュールから参照される可能性のある関数のダミー実装
int gpiod_line_request_output(struct gpiod_line *, const char *, int) {
  return 0;
}
int gpiod_line_set_value(struct gpiod_line *, int) { return 0; }
} // extern "C"

// -----------------------------------------------------------------------------
// テストケース
// -----------------------------------------------------------------------------

namespace device_reminder {

// 正常系: GPIOReader が 1 を読み取る
TEST(GPIOReaderIntegrationTest, ReadReturnsTrueWhenHigh) {
  ::testing::StrictMock<MockGpiod> mock;
  g_gpiod_mock = &mock;

  // ロガーの準備
  auto sink = std::make_shared<spdlog::sinks::null_sink_mt>();
  auto spd = std::make_shared<spdlog::logger>("null", sink);
  auto logger = std::make_shared<Logger>(spd);

  struct gpiod_chip chip_obj;
  struct gpiod_line line_obj;

  {
    InSequence seq;
    EXPECT_CALL(mock, chip_open(StrEq("/dev/gpiochip0")))
        .WillOnce(Return(&chip_obj));
    EXPECT_CALL(mock, chip_get_line(&chip_obj, 1)).WillOnce(Return(&line_obj));
    EXPECT_CALL(mock, line_request_input(&line_obj, _)).WillOnce(Return(0));
    EXPECT_CALL(mock, line_get_value(&line_obj)).WillOnce(Return(1));
    EXPECT_CALL(mock, line_release(&line_obj));
    EXPECT_CALL(mock, chip_close(&chip_obj));
  }

  bool result = false;
  {
    GPIOReader reader(logger, 1);
    result = reader.read();
  }

  EXPECT_TRUE(result);
  g_gpiod_mock = nullptr;
}

// 異常系: 無効なピン番号で初期化失敗
TEST(GPIOReaderIntegrationTest, ConstructorThrowsWhenGetLineFails) {
  ::testing::StrictMock<MockGpiod> mock;
  g_gpiod_mock = &mock;

  auto sink = std::make_shared<spdlog::sinks::null_sink_mt>();
  auto spd = std::make_shared<spdlog::logger>("null", sink);
  auto logger = std::make_shared<Logger>(spd);

  struct gpiod_chip chip_obj;

  {
    InSequence seq;
    EXPECT_CALL(mock, chip_open(StrEq("/dev/gpiochip0")))
        .WillOnce(Return(&chip_obj));
    EXPECT_CALL(mock, chip_get_line(&chip_obj, _)).WillOnce(Return(nullptr));
    EXPECT_CALL(mock, chip_close(&chip_obj));
  }

  EXPECT_THROW({ GPIOReader reader(logger, -1); }, std::runtime_error);

  g_gpiod_mock = nullptr;
}

} // namespace device_reminder
