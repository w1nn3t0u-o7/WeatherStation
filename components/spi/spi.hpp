/**
 * @file spi.hpp
 * @brief SPI communication class.
 * 
 * Provides an abstraction for SPI communication including device registration, 
 * configuration, and data transfer functionality.
 */

#pragma once

#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "com_protocols.hpp"

namespace MZDK
{
    /**
     * @brief Default MISO pin for SPI communication.
     */
    constexpr static int DEFAULT_SPI_MISO_PIN = 19;

    /**
     * @brief Default MOSI pin for SPI communication.
     */
    constexpr static int DEFAULT_SPI_MOSI_PIN = 23;

    /**
     * @brief Default SCLK pin for SPI communication.
     */
    constexpr static int DEFAULT_SPI_SCLK_PIN = 18;

    /**
     * @brief Default SPI mode.
     */
    constexpr static uint8_t MODE = 0;

    /**
     * @brief Default number of address bits.
     */
    constexpr static uint8_t ADDR_BITS = 7;

    /**
     * @brief Default number of command bits.
     */
    constexpr static uint8_t CMD_BITS = 1;

    /**
     * @brief SPI write command indicator.
     */
    constexpr static uint8_t SPI_WRITE = 0;

    /**
     * @brief SPI read command indicator.
     */
    constexpr static uint8_t SPI_READ = 1;

    /**
     * @class SPI
     * @brief SPI communication implementation extending ComProtocol.
     * 
     * This class provides methods for configuring and communicating via SPI, including
     * reading and writing registers, supporting multiple bytes, and device-specific initialization.
     */
    class SPI : public ComProtocol {
    private:
        spi_bus_config_t m_spi_bus_config {}; ///< SPI bus configuration.
        spi_device_interface_config_t m_spi_interface_config {}; ///< SPI device configuration.
        spi_device_handle_t m_spi_device_handle {}; ///< Handle for the SPI device.
        spi_host_device_t m_spi_peripheral {}; ///< SPI host device.
        spi_transaction_t m_spi_transfer {}; ///< SPI transaction configuration.

        /**
         * @brief Transfers a single byte over SPI.
         * @param reg_addr Register address.
         * @param data Data to transfer.
         * @param command Command to indicate read or write.
         * @return Status of the transfer.
         */
        int m_transferByte(const uint8_t reg_addr, const uint8_t data, const uint8_t command);

        /**
         * @brief Transfers multiple bytes over SPI.
         * @param reg_addr Register address.
         * @param tx_buf Transmit buffer.
         * @param rx_buf Receive buffer.
         * @param data_length Length of data to transfer.
         * @param command Command to indicate read or write.
         * @return Status of the transfer.
         */
        int m_transferMultiplesBytes(const uint8_t reg_addr, uint8_t* tx_buf, uint8_t* rx_buf, size_t data_length, const uint8_t command);

    public:
        /**
         * @brief Constructs an SPI object with optional pin configurations.
         * @param spi_peripheral SPI host device.
         * @param miso_pin MISO pin (default: DEFAULT_SPI_MISO_PIN).
         * @param mosi_pin MOSI pin (default: DEFAULT_SPI_MOSI_PIN).
         * @param sclk_pin SCLK pin (default: DEFAULT_SPI_SCLK_PIN).
         */
        SPI(const spi_host_device_t spi_peripheral, 
            const int miso_pin = DEFAULT_SPI_MISO_PIN,
            const int mosi_pin = DEFAULT_SPI_MOSI_PIN, 
            const int sclk_pin = DEFAULT_SPI_SCLK_PIN);

        /**
         * @brief Registers an SPI device.
         * @param mode SPI mode.
         * @param cs_pin Chip select pin.
         * @param addr_length Address length in bits.
         * @param command_length Command length in bits.
         * @param bus_speed SPI bus speed in Hz (default: 1000).
         * @return Status of device registration.
         */
        int registerDevice(const uint8_t mode, const int cs_pin, const int addr_length, const int command_length, const int bus_speed = 1000);

        /**
         * @brief Sets the SPI bus speed.
         * @param bus_speed Desired bus speed in Hz.
         */
        void setBusSpeed(const int bus_speed);

        /**
         * @brief Gets the current SPI bus speed.
         * @return Current bus speed in Hz.
         */
        int getBusSpeed();

        /**
         * @brief Gets the SPI device handle.
         * @return SPI device handle.
         */
        spi_device_handle_t getSpiDeviceHandle();

        /**
         * @brief Reads a register value.
         * @param reg_addr Register address.
         * @return Value read from the register.
         */
        uint8_t readRegister(const uint8_t reg_addr) override;

        /**
         * @brief Writes a value to a register.
         * @param reg_addr Register address.
         * @param tx_data Data to write.
         * @return Status of the write operation.
         */
        int writeRegister(const uint8_t reg_addr, const uint8_t tx_data) override;

        /**
         * @brief Writes multiple bytes to a register.
         * @param reg_addr Register address.
         * @param tx_data Data buffer to write.
         * @param length Length of data to write.
         * @return Status of the write operation.
         */
        int writeRegisterMultipleBytes(const uint8_t reg_addr, uint8_t *tx_data, const int length) override;

        /**
         * @brief Reads multiple bytes from a register.
         * @param reg_addr Register address.
         * @param rx_data Buffer to store received data.
         * @param length Number of bytes to read.
         * @return Status of the read operation.
         */
        int readRegisterMultipleBytes(const uint8_t reg_addr, uint8_t *rx_data, const int length) override;

        /**
         * @brief Reads two consecutive registers.
         * @param reg_addr Starting register address.
         * @return Combined value from the two registers.
         */
        int read2Registers(const uint8_t reg_addr) override;

        /**
         * @brief Initializes SPI for BME280 sensor.
         * @param cs_pin Chip select pin.
         * @return Status of the initialization.
         */
        int initSpiForBme280(const int cs_pin);
    }; 
}


