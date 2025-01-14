#include <iostream>

#include "spi.hpp"
#include "i2c.hpp"
#include "bme280.hpp"

constexpr static int BME280_CS_PIN = 5;

MZDK::I2C i2c(I2C_NUM_0, I2C_MODE_MASTER);
MZDK::SPI spi(SPI3_HOST);
MZDK::BME280 bme280(&spi);

extern "C" void app_main(void) {    
    spi.initSpiForBme280(BME280_CS_PIN);
    i2c.initI2cForBme280(0x76);
    
    bme280.init();
    bme280.setMode(1);
    bme280.setConfigFilter(1);

    float Temperature{};
    float Pressure{};
    int Humidity{};
    int Id{};

    while(true) {
        Id = bme280.getDeviceID();
        bme280.getAllResults(&Temperature, &Humidity, &Pressure);
        std::cout << "==================================================" << std::endl;
        std::cout << "SPI Temperature: " << Temperature << "c" << std::endl;
        std::cout << "SPI Humidity   : " << Humidity << "%" << std::endl;
        std::cout << "SPI Pressure   : " << Pressure << "Pa" << std::endl;
        std::cout << "SPI ID         : " << Id << std::endl;
        std::cout << "==================================================" << std::endl;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


