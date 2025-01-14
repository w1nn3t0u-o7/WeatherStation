#pragma once

#include "driver/i2c.h"

#include "com_protocols.hpp"

namespace MZDK {
    constexpr static int DEFAULT_I2C_SDA_PIN = 17;
    constexpr static int DEFAULT_I2C_SCL_PIN = 16;
    constexpr static uint32_t I2C_CLK_SPEED_HZ = 400000;

    class I2C : public ComProtocol {
    private:
        i2c_port_t m_port {};
        i2c_mode_t m_mode {};
        i2c_config_t _config {};
        uint32_t m_clk_speed {};

        uint8_t m_dev_addr{};

    public:
        I2C(i2c_port_t port, i2c_mode_t mode, int sda_pin = DEFAULT_I2C_SDA_PIN, int scl_pin = DEFAULT_I2C_SCL_PIN, bool sda_pullup_en = false, bool scl_pullup_en = false);
        ~I2C();

        uint8_t readRegister(uint8_t reg_addr);        
        int writeRegister(uint8_t reg_addr, uint8_t txData);
        int readRegisterMultipleBytes(uint8_t reg_addr, uint8_t *rx_data, int length);
        int writeRegisterMultipleBytes(uint8_t reg_addr, uint8_t *tx_data, int length);

        int writeByteData(const uint8_t reg, const uint8_t value) override;
        int readByteData(const uint8_t reg) override;
        int readWordData(const uint8_t reg) override;
        int readBlockData(const uint8_t reg, uint8_t *buf, const int length) override;

        void initI2cForBme280(const uint8_t dev_addr = 0x76);
    }; 
} 


