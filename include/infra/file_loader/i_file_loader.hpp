#pragma once

#include <string>

namespace device_reminder {

class IFileLoader {
public:
    virtual ~IFileLoader() = default;

    virtual int load_int(const std::string& key) const = 0;
};

} // namespace device_reminder
