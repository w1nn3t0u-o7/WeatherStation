/**
 * @file spi.hpp
 * @brief Implementation of SPI driver for MZDK library.
 */

#include "spi.hpp"

/**
 * @brief Enables the SPI peripheral clock and resets the peripheral.
 */
void MZDK::SPI::m_enablePeripheral() {
    if (m_spi == &SPI2) {
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI2_CLK_EN);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI2_RST);
    } else if (m_spi == &SPI3) {
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI3_CLK_EN);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI3_RST);
    }
}

/**
 * @brief Constructs the SPI object and initializes the SPI peripheral.
 * 
 * @param port SPI port number (2 or 3).
 * @param cs GPIO pin for chip select.
 * @param mosi GPIO pin for MOSI.
 * @param miso GPIO pin for MISO.
 * @param sclk GPIO pin for SCLK.
 */
MZDK::SPI::SPI(int port, GPIO cs, GPIO mosi, GPIO miso, GPIO sclk) 
    : m_cs(cs), m_mosi(mosi), m_miso(miso), m_sclk(sclk) {
    if (port == 2) {
        m_spi = &SPI2;
    } else if (port == 3) {
        m_spi = &SPI3;
    }
    m_enablePeripheral();
    init();
}

/**
 * @brief Initializes the SPI peripheral with default settings.
 */
void MZDK::SPI::init() {
    if (!m_spi) return;

    // Configure SPI clock
    m_spi->clock.val = 0; // Default clock settings
    m_spi->user.val = 0;
    m_spi->user1.val = 0;
    m_spi->ctrl.val = 0;

    // Set Master mode
    m_spi->slave.val = 0;
    m_spi->user.usr_miso_highpart = 0;
    m_spi->user.usr_mosi_highpart = 0;

    // Configure chip select
    m_spi->pin.cs0_dis = 0; // Activate CS0
}

/**
 * @brief Writes a value to the specified register via SPI.
 * 
 * @param reg The register address to write to.
 * @param data The value to write.
 * @return Always returns 0.
 */
uint8_t MZDK::SPI::write(uint8_t reg, uint8_t data) {
    // Prepare the data buffer
    m_spi->data_buf[0] = reg & 0x7F;
    m_spi->data_buf[1] = data;

    // Set transmission length
    m_spi->mosi_dlen.usr_mosi_dbitlen = 15;

    // Start transmission
    m_spi->cmd.usr = 1;

    // Wait for transmission to complete
    while (m_spi->cmd.usr);

    return 0;
}

/**
 * @brief Reads a value from the specified register via SPI.
 * 
 * @param reg The register address to read from.
 * @return The read value.
 */
uint8_t MZDK::SPI::read(uint8_t reg) {
    uint8_t data;

    // Prepare the buffer with the register address
    m_spi->data_buf[0] = reg | 0x80;

    // Set transmission length (1 byte sent, `len` bytes received)
    m_spi->mosi_dlen.usr_mosi_dbitlen = 7;         // 1 byte = 8 bits, -1 for the register
    m_spi->miso_dlen.usr_miso_dbitlen = 7;

    // Start transmission
    m_spi->cmd.usr = 1;

    // Wait for transmission to complete
    while (m_spi->cmd.usr);

    // Read data from the hardware buffer
    data = m_spi->data_buf[0];
    return data;
}

/**
 * @brief Reads a 16-bit word starting from the specified register.
 * 
 * @param reg The starting register address.
 * @return The 16-bit read value.
 */
int MZDK::SPI::readWord(uint8_t reg) {
    uint16_t data;
    data = read(reg) << 8 | read(reg + 1);
    return data;
}


