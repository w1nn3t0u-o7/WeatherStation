/**
 * @file i2c.hpp
 * @brief I2C communication class.
 * 
 * Provides an abstraction for I2C communication including device initialization, 
 * configuration, and data transfer.
 */

#pragma once

#include "driver/i2c.h"
#include "com_protocols.hpp"

namespace MZDK {
    /**
     * @brief Default SDA pin for I2C communication.
     */
    constexpr static int DEFAULT_I2C_SDA_PIN = 21;

    /**
     * @brief Default SCL pin for I2C communication.
     */
    constexpr static int DEFAULT_I2C_SCL_PIN = 22;

    /**
     * @brief I2C standard mode clock speed (100 kHz).
     */
    constexpr static uint32_t I2C_STANDARD_MODE_CLK_SPEED_HZ = 100000;

    /**
     * @brief I2C fast mode clock speed (400 kHz).
     */
    constexpr static uint32_t I2C_FAST_MODE_CLK_SPEED_HZ = 400000;

    /**
     * @class I2C
     * @brief I2C communication implementation extending ComProtocol.
     * 
     * This class provides methods for configuring and communicating via I2C, including
     * reading and writing registers, supporting multiple bytes, and device-specific initialization.
     */
    class I2C : public ComProtocol {
    private:
        i2c_port_t m_port {}; ///< I2C port identifier.
        i2c_mode_t m_mode {}; ///< I2C operation mode.
        i2c_config_t m_config {}; ///< I2C configuration parameters.
        uint32_t m_clk_speed {}; ///< I2C clock speed.

        uint8_t m_dev_addr {}; ///< Address of the I2C device.

    public:
        /**
         * @brief Constructs an I2C object.
         * @param port I2C port identifier.
         * @param mode I2C operation mode.
         * @param sda_pin SDA pin number (default: DEFAULT_I2C_SDA_PIN).
         * @param scl_pin SCL pin number (default: DEFAULT_I2C_SCL_PIN).
         * @param sda_pullup_en Enable SDA pull-up resistor (default: false).
         * @param scl_pullup_en Enable SCL pull-up resistor (default: false).
         */
        I2C(i2c_port_t port, i2c_mode_t mode, int sda_pin = DEFAULT_I2C_SDA_PIN, int scl_pin = DEFAULT_I2C_SCL_PIN, bool sda_pullup_en = false, bool scl_pullup_en = false);

        /**
         * @brief Destroys the I2C object.
         */
        ~I2C();
        
        /**
         * @brief Sets the I2C bus speed to standard mode (100 kHz).
         */
        void setStandardBusSpeed();

        /**
         * @brief Sets the I2C bus speed to fast mode (400 kHz).
         */
        void setFastBusSpeed();

        /**
         * @brief Reads a single register.
         * @param reg_addr Address of the register to read.
         * @return Value read from the register.
         */
        uint8_t readRegister(uint8_t reg_addr) override;        

        /**
         * @brief Writes a single register.
         * @param reg_addr Address of the register to write.
         * @param tx_data Data to write to the register.
         * @return Status of the write operation.
         */
        int writeRegister(uint8_t reg_addr, uint8_t tx_data) override;

        /**
         * @brief Reads multiple bytes from a register.
         * @param reg_addr Address of the register to read.
         * @param rx_data Buffer to store the read data.
         * @param length Number of bytes to read.
         * @return Status of the read operation.
         */
        int readRegisterMultipleBytes(const uint8_t reg_addr, uint8_t *rx_data, const int length) override;

        /**
         * @brief Writes multiple bytes to a register.
         * @param reg_addr Address of the register to write.
         * @param tx_data Buffer containing the data to write.
         * @param length Number of bytes to write.
         * @return Status of the write operation.
         */
        int writeRegisterMultipleBytes(const uint8_t reg_addr, uint8_t *tx_data, const int length) override;

        /**
         * @brief Reads two consecutive registers.
         * @param reg Address of the first register.
         * @return Combined value from the two registers.
         */
        int read2Registers(const uint8_t reg) override;

        /**
         * @brief Initializes the I2C interface for the BME280 sensor.
         * @param dev_addr Address of the BME280 device (default: 0x76).
         */
        void initI2cForBme280(const uint8_t dev_addr = 0x76);
    }; 
}


