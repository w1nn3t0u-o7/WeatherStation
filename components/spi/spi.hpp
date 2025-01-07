#pragma once

#include "gpio.hpp"
#include "com_protocols.hpp"
namespace MZDK {
    class SPI : public ComProtocol {
    private:
        GPIO* m_mosi_pin;
        GPIO* m_miso_pin;
        GPIO* m_sclk_pin;
        GPIO* m_cs_pin;
    public:
        SPI();// Konstruktor domyślny

        SPI(GPIO& mosi_gpio, GPIO& miso_gpio, GPIO& sclk_gpio, GPIO& cs_gpio);

        void write_byte(uint8_t byte) override;

        uint8_t read_byte() override;
    };
}