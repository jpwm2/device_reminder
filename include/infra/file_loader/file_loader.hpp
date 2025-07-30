#pragma once

#include "infra/file_loader/i_file_loader.hpp"
#include "infra/logger/i_logger.hpp"

#include <string>
#include <memory>

namespace device_reminder {

class FileLoader : public IFileLoader {
public:
    FileLoader(std::shared_ptr<ILogger> logger,
               const std::string& file_path,
               const std::string& key);

    int load_int() const override;
    std::string load_string() const override;
    std::vector<std::string> load_string_list() const override;

private:
    std::string load_value() const;

    std::shared_ptr<ILogger> logger_;
    std::string              file_path_;
    std::string              key_;
};

} // namespace device_reminder
