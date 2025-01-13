#pragma once

#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "com_protocols.hpp"

namespace MZDK
{
    constexpr static uint8_t MODE = 0;
    constexpr static uint8_t ADDR_BITS = 7;
    constexpr static uint8_t CMD_BITS = 1;

    constexpr static uint8_t SPI_WRITE = 0;
    constexpr static uint8_t SPI_READ = 1;

    class Spi :public ComProtocol
    {
    private:
        spi_bus_config_t _spi_bus_cfg{};
        spi_device_interface_config_t _spi_interface_cfg{};
        spi_device_handle_t _handle{};
        spi_host_device_t _spi_peripheral{};       
        
        spi_transaction_t _spi_transaction{};

        esp_err_t transferByte(const uint8_t reg_addr, const uint8_t data, const uint8_t command = 0);
        esp_err_t transferMultiplesBytes(const uint8_t reg_addr, uint8_t* tx_buf, uint8_t* rx_buf, size_t data_length, const uint8_t command = 0);

    public:
        esp_err_t Init(const spi_host_device_t spi_peripheral, const int pin_miso, const int pin_mosi, const int pin_sclk);
        esp_err_t RegisterDevice(const uint8_t mode, const int ss, const int addr_length, const int command_length, const int bus_speed = 1000);
        uint8_t ReadRegister(const uint8_t reg_addr, const uint8_t command = 0);
        esp_err_t WriteRegister(const uint8_t reg_addr, const uint8_t reg_data, const uint8_t command = 0);
        esp_err_t WriteRegisterMultipleBytes(const uint8_t reg_addr, uint8_t* reg_data_buffer, const uint8_t byte_count, const uint8_t command = 0);
        esp_err_t ReadRegisterMultipleBytes(const uint8_t reg_addr, uint8_t* reg_data_buffer, const uint8_t byte_count, const uint8_t command = 0);
        spi_device_handle_t GetHandle(void);

        esp_err_t writeByteData(const uint8_t reg, const uint8_t value) override;
        int readByteData(const uint8_t reg) override;
        int readWordData(const uint8_t reg) override;
        esp_err_t readBlockData(const uint8_t reg, uint8_t *buf, const int length) override;

        esp_err_t InitSpiForBme280(const int ss);
    }; // class Spi
} // namespace CPPSPI

