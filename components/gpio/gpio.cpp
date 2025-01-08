#include "gpio.hpp"

MZDK::GPIO::GPIO() : m_pin(-1), m_direction(INPUT) {}

MZDK::GPIO::GPIO(int pin, Direction direction) : m_pin(pin), m_direction(direction) {
    if (pin >= 0 && pin < 40) {
        if (direction == OUTPUT) {
            *regAccess(GPIO_ENABLE_W1TS_REG) |= BIT(pin);// Wrazie czego zmienić na BIT64
        } else {
            *regAccess(GPIO_ENABLE_W1TC_REG) |= BIT(pin);
        }
    }
}

void MZDK::GPIO::setPullMode(uint32_t reg, bool pull_up, bool pull_down) {
    uint32_t reg_value = *regAccess(reg);
    
    if (pull_up) {
        reg_value |= FUN_PU;  // Pull-up bit (7)
    } else {
        reg_value &= ~FUN_PU;
    }

    if (pull_down) {
        reg_value |= FUN_PD;  // Pull-down bit (6)
    } else {
        reg_value &= ~FUN_PD;
    }

    *regAccess(reg) = reg_value;

}

void MZDK::GPIO::setLevel(Level level) {
    if (m_direction == OUTPUT) {
        if (level == HIGH) {
            *regAccess(GPIO_OUT_W1TS_REG) |= BIT(m_pin);
        } else {
            *regAccess(GPIO_OUT_W1TC_REG) |= BIT(m_pin);
        }
    }
}

bool MZDK::GPIO::readInput() {
    return (*regAccess(GPIO_IN_REG) >> m_pin) & 0x1;
}

void MZDK::GPIO::enablePullUp() {
    if (m_direction == INPUT) {
        if (m_pin < 28 && m_pin > 31) {
            uint32_t reg = IO_MUX_GPIO_REG[m_pin]; // Pobierz rejestr IO_MUX dla pinu
            setPullMode(reg, true, false);   // Włącz pull-up
        } else {
            std::cout << "Error: This pin doesn't have pullup/pulldown resistor functionality" << std::endl;
        }
    }
}

void MZDK::GPIO::disablePullUp() {
    if (m_direction == INPUT) {
        if (m_pin < 28 && m_pin > 31) {
            uint32_t reg = IO_MUX_GPIO_REG[m_pin]; // Pobierz rejestr IO_MUX dla pinu
            setPullMode(reg, false, false);   // Włącz pull-up
        } else {
            std::cout << "Error: This pin doesn't have pullup/pulldown resistor functionality" << std::endl;
        }
    }
}

void MZDK::GPIO::enablePullDown() {
    if (m_direction == INPUT) {
        if (m_pin < 28 && m_pin > 31) {
            uint32_t reg = IO_MUX_GPIO_REG[m_pin]; // Pobierz rejestr IO_MUX dla pinu
            setPullMode(reg, false, true);   // Włącz pull-up
        } else {
            std::cout << "Error: This pin doesn't have pullup/pulldown resistor functionality" << std::endl;
        }
    }
}

void MZDK::GPIO::disablePullDown() {
    if (m_direction == INPUT) {
        if (m_pin < 28 && m_pin > 31) {
            uint32_t reg = IO_MUX_GPIO_REG[m_pin]; // Pobierz rejestr IO_MUX dla pinu
            setPullMode(reg, false, false);   // Włącz pull-up
        } else {
            std::cout << "Error: This pin doesn't have pullup/pulldown resistor functionality" << std::endl;
        }
    }
}

void MZDK::GPIO::enableOpenDrainMode() {
    if (m_direction == INPUT) {
        uint32_t reg = IO_MUX_GPIO_REG[m_pin]; // Pobierz rejestr IO_MUX dla pinu
        *regAccess(reg) |= GPIO_PIN_PAD_DRIVER;
    }
}

void MZDK::GPIO::disableOpenDrainMode() {
    if (m_direction == INPUT) {
        uint32_t reg = IO_MUX_GPIO_REG[m_pin]; // Pobierz rejestr IO_MUX dla pinu
        *regAccess(reg) &= ~GPIO_PIN_PAD_DRIVER;
    }
}
