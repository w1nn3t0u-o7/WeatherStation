#pragma once

#include "bme280_common.hpp"
#include "i2c.hpp"

namespace MZDK
{
    class BME280I2C : public BME280
    {
    private:
        MZDK::I2c *i2c;
        uint8_t _devAddress{};

    protected:
        esp_err_t writeByteData(const uint8_t reg, const uint8_t value) override;
        int readByteData(const uint8_t reg) override;
        int readWordData(const uint8_t reg) override;
        esp_err_t readBlockData(const uint8_t reg, uint8_t *buf, const int length) override;

    public:
        void InitI2c(MZDK::I2c *i_i2c, const uint8_t dev_addr = 0x76);
    }; // namespace CPPBME280
} // namespace CPPBME280