#pragma once

#include <string>
#include <vector>

namespace device_reminder {

class IFileLoader {
public:
    virtual ~IFileLoader() = default;

    virtual int load_int(const std::string& key) const = 0;
    virtual std::string load_string(const std::string& key) const = 0;
    virtual std::vector<std::string> load_string_list(const std::string& key) const = 0;
};

} // namespace device_reminder
