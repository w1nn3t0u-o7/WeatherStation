#pragma once

#include "gpio.hpp"
#include "com_protocols.hpp"

class SPI : public ComProtocol {
private:
    GPIO* mosi_pin;
    GPIO* miso_pin;
    GPIO* sclk_pin;
    GPIO* cs_pin;
public:
    SPI();// Konstruktor domyślny

    SPI(GPIO& mosi_gpio, GPIO& miso_gpio, GPIO& sclk_gpio, GPIO& cs_gpio);

    void write_byte(uint8_t byte) override;

    uint8_t read_byte() override;
};