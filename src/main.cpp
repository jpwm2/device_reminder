// main.cpp
#include "app_builder.hpp"

int main() {
    device_reminder::AppBuilder builder;
    auto app = builder.build();
    app->run();
}
