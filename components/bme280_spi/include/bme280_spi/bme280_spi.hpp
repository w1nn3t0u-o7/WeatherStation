#pragma once

#include "bme280_common/bme280_common.hpp"
#include "CPPSPI/cppspi.hpp"

namespace CPPBME280
{
    constexpr static uint8_t MODE = 0;
    constexpr static uint8_t ADDR_BITS = 7;
    constexpr static uint8_t CMD_BITS = 1;

    constexpr static uint8_t SPI_WRITE = 0;
    constexpr static uint8_t SPI_READ = 1;

    class BME280SPI : public BME280
    {
    private:
        CPPSPI::Spi *spi;

    protected:
        esp_err_t writeByteData(const uint8_t reg, const uint8_t value) override;
        int readByteData(const uint8_t reg) override;
        int readWordData(const uint8_t reg) override;
        esp_err_t readBlockData(const uint8_t reg, uint8_t *buf, const int length) override;

    public:
        esp_err_t InitSpi(CPPSPI::Spi *i_spi, const int ss);
    }; // namespace CPPBME280
}