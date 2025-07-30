#pragma once

#include "infra/file_loader/i_file_loader.hpp"
#include "infra/logger/i_logger.hpp"

#include <string>
#include <unordered_map>
#include <memory>

namespace device_reminder {

class FileLoader : public IFileLoader {
public:
    FileLoader(const std::string& file_path, std::shared_ptr<ILogger> logger = nullptr);

    int load_int(const std::string& key) const override;
    std::string load_string(const std::string& key) const override;

private:
    void parse_file(const std::string& file_path);

    std::unordered_map<std::string, std::string> values_;
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder
