#pragma once

#include "driver/spi_common.h"
#include "driver/spi_master.h"

#include "com_protocols.hpp"

namespace MZDK
{
    constexpr static int DEFAULT_SPI_MISO_PIN = 19;
    constexpr static int DEFAULT_SPI_MOSI_PIN = 23;
    constexpr static int DEFAULT_SPI_SCLK_PIN = 18;

    constexpr static uint8_t MODE = 0;
    constexpr static uint8_t ADDR_BITS = 7;
    constexpr static uint8_t CMD_BITS = 1;

    constexpr static uint8_t SPI_WRITE = 0;
    constexpr static uint8_t SPI_READ = 1;

    class SPI :public ComProtocol {
    private:
        spi_bus_config_t m_spi_bus_config {};
        spi_device_interface_config_t m_spi_interface_config {};
        spi_device_handle_t m_spi_device_handle {};
        spi_host_device_t m_spi_peripheral {};       
        spi_transaction_t m_spi_transfer {};

        int m_transferByte(const uint8_t reg_addr, const uint8_t data, const uint8_t command);
        int m_transferMultiplesBytes(const uint8_t reg_addr, uint8_t* tx_buf, uint8_t* rx_buf, size_t data_length, const uint8_t command);

    public:
        SPI(const spi_host_device_t spi_peripheral, 
            const int miso_pin = DEFAULT_SPI_MISO_PIN,
            const int mosi_pin = DEFAULT_SPI_MOSI_PIN, 
            const int sclk_pin = DEFAULT_SPI_SCLK_PIN);

        int registerDevice(const uint8_t mode, const int cs_pin, const int addr_length, const int command_length, const int bus_speed = 1000);
        void setBusSpeed(const int bus_speed);
        int getBusSpeed();
        spi_device_handle_t getSpiDeviceHandle();

        uint8_t readRegister(const uint8_t reg_addr) override;
        int writeRegister(const uint8_t reg_addr, const uint8_t tx_data) override;
        int writeRegisterMultipleBytes(const uint8_t reg_addr, uint8_t *tx_data, const int length) override;
        int readRegisterMultipleBytes(const uint8_t reg_addr, uint8_t *rx_data, const int length) override;

        int read2Registers(const uint8_t reg_addr) override;

        int initSpiForBme280(const int cs_pin);
    }; 
} 

