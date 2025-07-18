// buzzer_driver_impl.hpp
#pragma once
#include "i_buzzer_driver.hpp"
#include <iostream>

namespace device_reminder {

class BuzzerDriver : public IBuzzerDriver {
public:
    void start_buzzer() override {
        std::cout << "Start Buzzer\n";
    }
    void stop_buzzer() override {
        std::cout << "Stop Buzzer\n";
    }
};

} // namespace device_reminder
