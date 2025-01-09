/**
 * @file spi.hpp
 * @brief SPI communication class for the MZDK library.
 */

#pragma once

#include "soc/spi_struct.h"
#include "soc/spi_reg.h"
#include "soc/dport_reg.h"
#include <cstring>

#include "com_protocols.hpp"
#include "gpio.hpp"

namespace MZDK {

/**
 * @class SPI
 * @brief SPI driver implementing the ComProtocol interface.
 */
class SPI : public ComProtocol {
private:
    spi_dev_t *m_spi; ///< Pointer to the SPI hardware structure.
    GPIO m_cs;        ///< GPIO pin for chip select.
    GPIO m_mosi;      ///< GPIO pin for MOSI.
    GPIO m_miso;      ///< GPIO pin for MISO.
    GPIO m_sclk;      ///< GPIO pin for SCLK.

    /**
     * @brief Enables the SPI peripheral.
     */
    void m_enablePeripheral() override;

public:
    /**
     * @brief Constructs the SPI object and sets up GPIO pins.
     * @param spi_num SPI port number (2 or 3).
     * @param cs Chip select GPIO pin.
     * @param mosi MOSI GPIO pin.
     * @param miso MISO GPIO pin.
     * @param sclk SCLK GPIO pin.
     */
    SPI(int spi_num, GPIO cs, GPIO mosi, GPIO miso, GPIO sclk);

    /**
     * @brief Initializes the SPI peripheral with default settings.
     */
    void init();

    /**
     * @brief Writes data to a register via SPI.
     * @param reg Register address.
     * @param data Data to write.
     * @return Status of the write operation.
     */
    uint8_t write(uint8_t reg, uint8_t data) override;

    /**
     * @brief Reads data from a register via SPI.
     * @param reg Register address.
     * @return Data read from the register.
     */
    uint8_t read(uint8_t reg) override;

    /**
     * @brief Reads a 16-bit value from consecutive registers via SPI.
     * @param reg Starting register address.
     * @return 16-bit data read from the registers.
     */
    int readWord(uint8_t reg) override;
};

}
