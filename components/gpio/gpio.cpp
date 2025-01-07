#include "gpio.hpp"

MZDK::GPIO::GPIO() : m_pin(-1), m_direction(Direction::INPUT) {}

MZDK::GPIO::GPIO(int pin, Direction direction) : m_pin(pin), m_direction(direction) {
    if (direction == Direction::OUTPUT) {
        *reg(GPIO_ENABLE_W1TS_REG) = (1ULL << pin);
    } else {
        *reg(GPIO_ENABLE_W1TC_REG) = (1ULL << pin);
    }
}

void MZDK::GPIO::write(bool value) {
    if (m_direction == Direction::OUTPUT) {
        if (value) {
            *reg(GPIO_OUT_W1TS_REG) = (1ULL << m_pin);
        } else {
            *reg(GPIO_OUT_W1TC_REG) = (1ULL << m_pin);
        }
    }
}

bool MZDK::GPIO::read() {
    return (*reg(GPIO_IN_REG) >> m_pin) & 0x1;
}
