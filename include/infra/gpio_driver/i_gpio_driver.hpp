#pragma once

#include <string>

namespace device_reminder {

enum class Direction { INPUT, OUTPUT };
enum class EdgeType { RISING, FALLING, BOTH };

class IGPIODriver {
public:
    virtual ~IGPIODriver() = default;

    virtual void write(bool value) = 0;
    virtual bool read() = 0;
    virtual void setEdge(EdgeType edge) = 0;
    virtual void waitForEdge() = 0;
};

} // namespace device_reminder
