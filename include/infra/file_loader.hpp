#pragma once

#include "infra/file_loader/i_file_loader.hpp"
#include "infra/logger/i_logger.hpp"

#include <string>
#include <memory>

namespace device_reminder {

class FileLoader : public IFileLoader {
public:
    FileLoader(std::shared_ptr<ILogger> logger,
               const std::string& file_path);

    int load_int(const std::string& key) const override;
    std::string load_string(const std::string& key) const override;
    std::vector<std::string> load_string_list(const std::string& key) const override;

private:
    std::string load_value(const std::string& key) const;

    std::shared_ptr<ILogger> logger_;
    std::string              file_path_;
};

} // namespace device_reminder
