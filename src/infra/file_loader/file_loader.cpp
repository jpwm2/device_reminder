#include "infra/file_loader/file_loader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace device_reminder {

FileLoader::FileLoader(std::shared_ptr<ILogger> logger,
                       const std::string& file_path,
                       const std::string& key)
    : logger_(std::move(logger))
    , file_path_(file_path)
    , key_(key)
{}

int FileLoader::load_int() const
{
    std::string value = load_value();
    try {
        return std::stoi(value);
    } catch (const std::exception&) {
        if (logger_) logger_->error(std::string("invalid int value for ") + key_);
        throw;
    }
}

std::string FileLoader::load_string() const
{
    return load_value();
}

std::vector<std::string> FileLoader::load_string_list() const
{
    std::string value = load_value();
    std::vector<std::string> result;
    std::istringstream iss(value);
    std::string item;
    while (std::getline(iss, item, ',')) {
        auto start = item.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        auto end = item.find_last_not_of(" \t\r\n");
        result.push_back(item.substr(start, end - start + 1));
    }
    return result;
}

std::string FileLoader::load_value() const
{
    std::ifstream ifs(file_path_);
    if (!ifs) {
        if (logger_) logger_->error("failed to open file: " + file_path_);
        throw std::runtime_error("failed to open file: " + file_path_);
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
        std::string k = trimmed.substr(0, eq_pos);
        std::string value = trimmed.substr(eq_pos + 1);

        auto key_end = k.find_last_not_of(" \t");
        if (key_end != std::string::npos) k = k.substr(0, key_end + 1);
        auto value_start = value.find_first_not_of(" \t");
        if (value_start != std::string::npos) value = value.substr(value_start);

        if (k == key_) {
            return value;
        }
    }
    if (logger_) logger_->error("key not found: " + key_);
    throw std::runtime_error("key not found: " + key_);
}

} // namespace device_reminder

