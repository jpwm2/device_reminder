#pragma once

#include "app/app.hpp"

namespace device_reminder {
class AppBuilder {
public:
    std::unique_ptr<App> build();
};
}
