#include "infra/gpio_operation/gpio_reader.hpp"

#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <fstream>
#include <stdexcept>
#include <string>

namespace device_reminder {

GPIOReader::GPIOReader(std::shared_ptr<ILogger> logger,
                       int pin_no,
                       std::shared_ptr<IFileLoader> chip_loader)
    : logger_(std::move(logger)),
      pin_no_(pin_no),
      chip_loader_(std::move(chip_loader)) {
    if (!chip_loader_) {
        if (logger_) logger_->error("chip loader is null");
        throw std::runtime_error("chip loader is null");
    }
    chip_name_ = chip_loader_->load_string();
    if (logger_) logger_->info("gpio chip: " + chip_name_);
}

bool GPIOReader::read() {
    const std::string path = "/sys/class/gpio/gpio" + std::to_string(pin_no_) + "/value";
    std::ifstream ifs(path);
    if (!ifs) {
        if (logger_) logger_->error("failed to open: " + path);
        throw std::runtime_error("failed to open: " + path);
    }
    char c = '0';
    ifs >> c;
    if (ifs.fail()) {
        if (logger_) logger_->error("failed to read: " + path);
        throw std::runtime_error("failed to read: " + path);
    }
    if (logger_) {
        logger_->info("GPIO" + std::to_string(pin_no_) + " value=" + std::string(1, c));
    }
    return c == '1';
}

void GPIOReader::poll_edge(bool detect_rising) {
    const std::string base = "/sys/class/gpio/gpio" + std::to_string(pin_no_);
    const std::string edge_path = base + "/edge";
    const std::string value_path = base + "/value";

    // configure edge
    {
        std::ofstream ofs(edge_path);
        if (!ofs) {
            if (logger_) logger_->error("failed to open: " + edge_path);
            throw std::runtime_error("failed to open: " + edge_path);
        }
        ofs << (detect_rising ? "rising" : "falling");
    }

    int fd = ::open(value_path.c_str(), O_RDONLY);
    if (fd < 0) {
        if (logger_) logger_->error("failed to open: " + value_path);
        throw std::runtime_error("failed to open: " + value_path);
    }

    // dummy read to clear any pending state
    char buf;
    ::read(fd, &buf, 1);

    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLPRI;

    int ret = ::poll(&pfd, 1, -1);
    ::close(fd);
    if (ret < 0) {
        if (logger_) logger_->error("poll failed on: " + value_path);
        throw std::runtime_error("poll failed");
    }
    if (logger_) {
        logger_->info(std::string("edge detected on GPIO") + std::to_string(pin_no_) +
                      (detect_rising ? " rising" : " falling"));
    }
}

} // namespace device_reminder

