#include <iostream>

#include "spi.hpp"
#include "i2c.hpp"
#include "bme280.hpp"

constexpr static int BME280_CS_PIN = 5;

MZDK::I2C i2c(I2C_NUM_0, I2C_MODE_MASTER);
MZDK::SPI spi(SPI3_HOST);
MZDK::BME280 bme280(&spi);

extern "C" void app_main(void)
{    
    // Register BME280 device as SPI device using GPIO5 as the select pin
    spi.initSpiForBme280(BME280_CS_PIN);
    // Initialise the I2C for BME280
    i2c.initI2cForBme280(0x76);
    
    // Initialize the BME280 device
    bme280.Init();
    bme280.SetMode(1);
    bme280.SetConfigFilter(1);

    float Temperature{};
    float Pressure{};
    int Humidity{};
    int Id{};

    while(true)
    {
        Id = bme280.GetDeviceID();
        bme280.GetAllResults(&Temperature, &Humidity, &Pressure);
        std::cout << "==================================================\n";
        std::cout << "SPI Temperature: " << Temperature << "c\n";
        std::cout << "SPI Humidity   : " << Humidity << "%\n";
        std::cout << "SPI Pressure   : " << Pressure << "Pa\n";
        std::cout << "SPI ID         : " << Id << '\n';
        std::cout << "==================================================\n";
        std::cout << "--------------------------------------------------\n";
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


