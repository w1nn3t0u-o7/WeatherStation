#pragma once

#include "gpio.hpp"
#include "com_protocols.hpp"

namespace MZDK {
    class I2C : public ComProtocol {
    private:
        GPIO* m_sda_pin;
        GPIO* m_scl_pin;
    public:
        I2C(); // Konstruktor domyślny

        I2C(GPIO& sda_gpio, GPIO& scl_gpio);

        void start();

        void stop();

        void write_byte(uint8_t byte) override;

        uint8_t read_byte() override;
    };
}