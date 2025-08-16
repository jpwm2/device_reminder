#pragma once

#include "core/app/app.hpp"
#include <memory>

namespace device_reminder {
class AppBuilder {
public:
    std::unique_ptr<App> build();
};
}
