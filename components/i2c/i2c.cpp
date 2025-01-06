#include "i2c.hpp"

I2C::I2C() : sda_pin(nullptr), scl_pin(nullptr) {}

I2C::I2C(GPIO& sda_gpio, GPIO& scl_gpio) : sda_pin(&sda_gpio), scl_pin(&scl_gpio) {
        // Ustawienie pinów GPIO jako wyjściowe
        sda_pin->write(false);  // Start jako LOW
        scl_pin->write(false);  // Start jako LOW
}

void I2C::start() {
        sda_pin->write(false);
        delay_us(2);
        scl_pin->write(false);
}

void I2C::stop() {
        scl_pin->write(false);
        delay_us(2);
        sda_pin->write(true);
        delay_us(2);
}

void I2C::write_byte(uint8_t byte) {
        for (int i = 0; i < 8; ++i) {
            if (byte & 0x80) {
                sda_pin->write(true);
            } else {
                sda_pin->write(false);
            }
            delay_us(2);
            scl_pin->write(true);
            delay_us(2);
            scl_pin->write(false);
            byte <<= 1;
        }
        // Acknowledge
        sda_pin->write(false);
        scl_pin->write(true);
        delay_us(2);
        scl_pin->write(false);
}

uint8_t I2C::read_byte() {
        uint8_t byte = 0;
        for (int i = 0; i < 8; ++i) {
            scl_pin->write(true);
            delay_us(2);
            byte = (byte << 1) | (sda_pin->read() ? 1 : 0);
            scl_pin->write(false);
        }
        return byte;
}