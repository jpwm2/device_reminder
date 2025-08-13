#pragma once

#include "infra/logger.hpp"

#include <memory>
#include <string>
#include <vector>

namespace device_reminder {

class IFileLoader {
public:
    virtual ~IFileLoader() = default;

    virtual int load_int(const std::string& file_path, const std::string& key) const = 0;
    virtual std::string load_string(const std::string& file_path, const std::string& key) const = 0;
    virtual std::vector<std::string> load_string_list(const std::string& file_path, const std::string& key) const = 0;
};

class FileLoader : public IFileLoader {
public:
    explicit FileLoader(std::shared_ptr<ILogger> logger);

    int load_int(const std::string& file_path, const std::string& key) const override;
    std::string load_string(const std::string& file_path, const std::string& key) const override;
    std::vector<std::string> load_string_list(const std::string& file_path, const std::string& key) const override;

private:
    std::string load_value(const std::string& file_path, const std::string& key) const;

    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder
