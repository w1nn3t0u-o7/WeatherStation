#include "spi.hpp"

SPI::SPI() : mosi_pin(nullptr), miso_pin(nullptr), sclk_pin(nullptr), cs_pin(nullptr) {} // Konstruktor domyślny

SPI::SPI(GPIO& mosi_gpio, GPIO& miso_gpio, GPIO& sclk_gpio, GPIO& cs_gpio)
        : mosi_pin(&mosi_gpio), miso_pin(&miso_gpio), sclk_pin(&sclk_gpio), cs_pin(&cs_gpio) {
        cs_pin->write(true); // CS high to deselect
}

void SPI::write_byte(uint8_t byte) {
    for (int i = 0; i < 8; ++i) {
        if (byte & 0x80) {
            mosi_pin->write(true);
        } else {
            mosi_pin->write(false);
        }
        delay_us(1);
        sclk_pin->write(true);
        delay_us(1);
        sclk_pin->write(false);
        byte <<= 1;
    }
}

uint8_t SPI::read_byte() {
    uint8_t byte = 0;
    for (int i = 0; i < 8; ++i) {
        sclk_pin->write(true);
        delay_us(1);
        byte = (byte << 1) | (miso_pin->read() ? 1 : 0);
        sclk_pin->write(false);
        delay_us(1);
    }
    return byte;
}