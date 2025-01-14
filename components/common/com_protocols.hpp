/**
 * @file com_protocol.hpp
 * @brief Abstract communication protocol interface.
 * 
 * Defines a generic interface for communication protocols with methods for reading 
 * and writing registers, supporting single and multiple bytes.
 */

#pragma once

namespace MZDK {
    /**
     * @class ComProtocol
     * @brief Abstract base class for communication protocols.
     * 
     * Provides a uniform interface for implementing specific communication protocols,
     * such as I2C or SPI.
     */
    class ComProtocol {
    public:
        /**
         * @brief Reads a single register.
         * @param reg_addr Address of the register to read.
         * @return Value read from the register.
         */
        virtual uint8_t readRegister(const uint8_t reg_addr) = 0;

        /**
         * @brief Writes a single register.
         * @param reg_addr Address of the register to write.
         * @param tx_data Data to write to the register.
         * @return Status of the write operation.
         */
        virtual int writeRegister(const uint8_t reg_addr, const uint8_t tx_data) = 0;

        /**
         * @brief Reads multiple bytes from a register.
         * @param reg_addr Address of the register to read.
         * @param rx_data Buffer to store the read data.
         * @param length Number of bytes to read.
         * @return Status of the read operation.
         */
        virtual int readRegisterMultipleBytes(const uint8_t reg_addr, uint8_t *rx_data, const int length) = 0;

        /**
         * @brief Writes multiple bytes to a register.
         * @param reg_addr Address of the register to write.
         * @param tx_data Buffer containing the data to write.
         * @param length Number of bytes to write.
         * @return Status of the write operation.
         */
        virtual int writeRegisterMultipleBytes(const uint8_t reg_addr, uint8_t *tx_data, const int length) = 0;

        /**
         * @brief Reads two consecutive registers.
         * @param reg_addr Address of the first register.
         * @return Combined value from the two registers.
         */
        virtual int read2Registers(const uint8_t reg_addr) = 0;

        /**
         * @brief Virtual destructor for the base class.
         */
        virtual ~ComProtocol() {}
    };
}
