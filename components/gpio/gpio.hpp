#pragma once

#include "registers.hpp"

enum class Direction { INPUT, OUTPUT };

namespace MZDK {
    class GPIO {
    private:
        int m_pin;
        Direction m_direction;
    public:
        GPIO();
        GPIO(int pin, Direction direction);

        void write(bool value);
        bool read();
    };
}