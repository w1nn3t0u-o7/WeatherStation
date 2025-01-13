#pragma once

#include "driver/i2c.h"
#include "com_protocols.hpp"

namespace MZDK
{
    class I2c : public ComProtocol
    {
    private:
        uint16_t _slaveAddr{};
        i2c_port_t _port{};
        i2c_mode_t _mode{};
        size_t _slv_rx_buf_len{};
        size_t _slv_tx_buf_len{};
        int _intr_alloc_flags{};

        uint8_t _devAddress{};

    public:
        I2c(i2c_port_t port, size_t slv_rx_buf_len = 0, size_t slv_tx_buf_len = 0, int intr_alloc_flags = 0);
        ~I2c();

        esp_err_t InitMaster(int sda_io_num,
                             int scl_io_num,
                             uint32_t clk_speed,
                             bool sda_pullup_en = false,
                             bool scl_pullup_en = false,
                             uint32_t clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL);

        uint8_t ReadRegister(uint8_t dev_addr, uint8_t reg_addr);        
        esp_err_t WriteRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t txData);
        esp_err_t ReadRegisterMultipleBytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *rx_data, int length);
        esp_err_t WriteRegisterMultipleBytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *tx_data, int length);

        esp_err_t writeByteData(const uint8_t reg, const uint8_t value) override;
        int readByteData(const uint8_t reg) override;
        int readWordData(const uint8_t reg) override;
        esp_err_t readBlockData(const uint8_t reg, uint8_t *buf, const int length) override;

        void InitI2cForBme280(const uint8_t dev_addr = 0x76);
    }; // class I2c
} // namespace CPPI2C


