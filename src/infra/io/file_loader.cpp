#include "infra/file_loader/file_loader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace device_reminder {

FileLoader::FileLoader(const std::string& file_path, std::shared_ptr<ILogger> logger)
    : logger_(std::move(logger))
{
    parse_file(file_path);
}

int FileLoader::load_int(const std::string& key) const
{
    auto it = values_.find(key);
    if (it == values_.end()) {
        throw std::runtime_error("key not found: " + key);
    }
    try {
        return std::stoi(it->second);
    } catch (const std::exception&) {
        if (logger_) logger_->error(std::string("invalid int value for ") + key);
        throw;
    }
}

std::string FileLoader::load_string(const std::string& key) const
{
    auto it = values_.find(key);
    if (it == values_.end()) {
        throw std::runtime_error("key not found: " + key);
    }
    return it->second;
}

void FileLoader::parse_file(const std::string& file_path)
{
    std::ifstream ifs(file_path);
    if (!ifs) {
        if (logger_) logger_->error("failed to open file: " + file_path);
        throw std::runtime_error("failed to open file: " + file_path);
    }

    std::string line;
    while (std::getline(ifs, line)) {
        auto comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        auto start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        auto end = line.find_last_not_of(" \t\r\n");
        auto trimmed = line.substr(start, end - start + 1);

        auto eq_pos = trimmed.find('=');
        if (eq_pos == std::string::npos) {
            if (logger_) logger_->error("invalid line: " + trimmed);
            continue;
        }
        std::string key = trimmed.substr(0, eq_pos);
        std::string value = trimmed.substr(eq_pos + 1);

        auto key_end = key.find_last_not_of(" \t");
        if (key_end != std::string::npos) key = key.substr(0, key_end + 1);
        auto value_start = value.find_first_not_of(" \t");
        if (value_start != std::string::npos) value = value.substr(value_start);

        values_[key] = value;
    }
}

} // namespace device_reminder
