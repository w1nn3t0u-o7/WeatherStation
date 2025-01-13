/**
 * @file spi.hpp
 * @brief SPI communication class for the MZDK library.
 */

#pragma once

#include "soc/spi_struct.h"
#include "soc/spi_reg.h"
#include "soc/dport_reg.h"
#include "soc/gpio_struct.h"
#include <cstring>

#include "com_protocols.hpp"
//#include "gpio.hpp"

namespace MZDK {

/**
 * @class SPI
 * @brief SPI driver implementing the ComProtocol interface.
 */
class SPI : public ComProtocol {
private:
        spi_dev_t* _spi_dev;
        uint32_t _clock_div;
        int _pin_miso;
        int _pin_mosi;
        int _pin_sclk;
        int _pin_cs;

        void configurePins();
        void resetSpi();

        int transferByte(const uint8_t reg_addr, const uint8_t data, uint8_t& rx_data, const uint8_t command = 0);
        int transferMultipleBytes(const uint8_t reg_addr, uint8_t* tx_buf, uint8_t* rx_buf, size_t data_length, const uint8_t command = 0);

        int readByte(const uint8_t reg_addr);
        int writeByte(const uint8_t reg_addr, const uint8_t data);

    public:
        int Init(spi_dev_t* spi_dev, const int pin_miso, const int pin_mosi, const int pin_sclk, const int pin_cs, uint32_t clock_div);
        uint8_t readRegister(const uint8_t reg_addr) override;
        uint16_t readWord(const uint8_t reg_addr) override;
        int writeRegister(const uint8_t reg_addr, const uint8_t reg_data) override;
};
}

