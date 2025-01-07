#include "i2c.hpp"

MZDK::I2C::I2C() : m_sda_pin(nullptr), m_scl_pin(nullptr) {}

MZDK::I2C::I2C(GPIO& sda_gpio, GPIO& scl_gpio) : m_sda_pin(&sda_gpio), m_scl_pin(&scl_gpio) {
    // Ustawienie pinów GPIO jako wyjściowe
    m_sda_pin->write(false);  // Start jako LOW
    m_scl_pin->write(false);  // Start jako LOW
}

void MZDK::I2C::start() {
    m_sda_pin->write(false);
    delay_us(2);
    m_scl_pin->write(false);
}

void MZDK::I2C::stop() {
    m_scl_pin->write(false);
    delay_us(2);
    m_sda_pin->write(true);
    delay_us(2);
}

void MZDK::I2C::write_byte(uint8_t byte) {
    for (int i = 0; i < 8; ++i) {
        if (byte & 0x80) {
            m_sda_pin->write(true);
        } else {
            m_sda_pin->write(false);
        }
        delay_us(2);
        m_scl_pin->write(true);
        delay_us(2);
        m_scl_pin->write(false);
        byte <<= 1;
    }
    // Acknowledge
    m_sda_pin->write(false);
    m_scl_pin->write(true);
    delay_us(2);
    m_scl_pin->write(false);
}

uint8_t MZDK::I2C::read_byte() {
    uint8_t byte = 0;
    for (int i = 0; i < 8; ++i) {
        m_scl_pin->write(true);
        delay_us(2);
        byte = (byte << 1) | (m_sda_pin->read() ? 1 : 0);
        m_scl_pin->write(false);
    }
    return byte;
}