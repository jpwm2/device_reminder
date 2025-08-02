#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/sinks/ringbuffer_sink.h>

#include <filesystem>
#include <fstream>
#include <memory>

#include "infra/buzzer_driver/buzzer_driver.hpp"
#include "infra/file_loader/file_loader.hpp"
#include "infra/logger/logger.hpp"
#include "infra/gpio_operation/gpio_setter/gpio_setter.hpp"

#include "tests/stubs/gpiod_stub.h"

using namespace device_reminder;
using ::testing::Contains;
using ::testing::Not;

class BuzzerDriverIntegration : public ::testing::Test {
protected:
    void SetUp() override {
        gpiod_stub_reset();

        config_path = std::filesystem::temp_directory_path() / "buzzer.cfg";
        std::ofstream ofs(config_path);
        ofs << "buzz_duration_ms=100\n";

        sink = std::make_shared<spdlog::sinks::ringbuffer_sink_mt>(32);
        spd_logger = std::make_shared<spdlog::logger>("test", sink);
        spd_logger->set_pattern("%v");
        spd_logger->set_level(spdlog::level::info);
        logger = std::make_shared<Logger>(spd_logger);
        loader = std::make_shared<FileLoader>(logger, config_path.string());
        gpio = std::make_shared<GPIOSetter>(logger, 4);
    }

    void TearDown() override {
        std::filesystem::remove(config_path);
    }

    std::filesystem::path                                config_path;
    std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt>   sink;
    std::shared_ptr<spdlog::logger>                      spd_logger;
    std::shared_ptr<Logger>                              logger;
    std::shared_ptr<FileLoader>                          loader;
    std::shared_ptr<GPIOSetter>                          gpio;
};

TEST_F(BuzzerDriverIntegration, OnOff正常系) {
    BuzzerDriver driver(loader, logger, gpio);

    EXPECT_NO_THROW(driver.on());
    EXPECT_NO_THROW(driver.off());

    auto logs = sink->last_formatted();
    EXPECT_THAT(logs, Contains(testing::HasSubstr("buzzer on")));
    EXPECT_THAT(logs, Contains(testing::HasSubstr("buzzer off")));
}

TEST_F(BuzzerDriverIntegration, On異常系_gpiod書き込み失敗) {
    BuzzerDriver driver(loader, logger, gpio);

    gpiod_stub_set_set_value_result(-1);

    EXPECT_THROW(driver.on(), std::runtime_error);

    auto logs = sink->last_formatted();
    EXPECT_THAT(logs, Not(Contains(testing::HasSubstr("buzzer on"))));
}

