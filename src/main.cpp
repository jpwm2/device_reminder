// main.cpp
#include "core/app_builder/app_builder.hpp"

int main() {
    device_reminder::AppBuilder builder;
    auto app = builder.build();
    app->run();
}
