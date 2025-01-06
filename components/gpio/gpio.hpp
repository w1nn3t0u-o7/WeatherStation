#pragma once

#include "registers.hpp"

class GPIO {
public:
    enum class Direction { INPUT, OUTPUT };

    GPIO();
    GPIO(int pin, Direction direction);

    void write(bool value);
    bool read();

private:
    int pin;
    Direction direction;
};