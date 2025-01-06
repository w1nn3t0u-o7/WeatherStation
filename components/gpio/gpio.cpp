#include "gpio.hpp"

GPIO::GPIO() : pin(-1), direction(Direction::INPUT) {}

GPIO::GPIO(int pin, Direction direction) : pin(pin), direction(direction) {
    if (direction == Direction::OUTPUT) {
        *reg(GPIO_ENABLE_W1TS_REG) = (1ULL << pin);
    } else {
        *reg(GPIO_ENABLE_W1TC_REG) = (1ULL << pin);
    }
}

void GPIO::write(bool value) {
    if (direction == Direction::OUTPUT) {
        if (value) {
            *reg(GPIO_OUT_W1TS_REG) = (1ULL << pin);
        } else {
            *reg(GPIO_OUT_W1TC_REG) = (1ULL << pin);
        }
    }
}

bool GPIO::read() {
    return (*reg(GPIO_IN_REG) >> pin) & 0x1;
}
