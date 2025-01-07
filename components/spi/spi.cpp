#include "spi.hpp"

MZDK::SPI::SPI() : m_mosi_pin(nullptr), m_miso_pin(nullptr), m_sclk_pin(nullptr), m_cs_pin(nullptr) {} // Konstruktor domyślny

MZDK::SPI::SPI(GPIO& mosi_gpio, GPIO& miso_gpio, GPIO& sclk_gpio, GPIO& cs_gpio)
        : m_mosi_pin(&mosi_gpio), m_miso_pin(&miso_gpio), m_sclk_pin(&sclk_gpio), m_cs_pin(&cs_gpio) {
        m_cs_pin->write(true); // CS high to deselect
}

void MZDK::SPI::write_byte(uint8_t byte) {
    for (int i = 0; i < 8; ++i) {
        if (byte & 0x80) {
            m_mosi_pin->write(true);
        } else {
            m_mosi_pin->write(false);
        }
        delay_us(1);
        m_sclk_pin->write(true);
        delay_us(1);
        m_sclk_pin->write(false);
        byte <<= 1;
    }
}

uint8_t MZDK::SPI::read_byte() {
    uint8_t byte = 0;
    for (int i = 0; i < 8; ++i) {
        m_sclk_pin->write(true);
        delay_us(1);
        byte = (byte << 1) | (m_miso_pin->read() ? 1 : 0);
        m_sclk_pin->write(false);
        delay_us(1);
    }
    return byte;
}