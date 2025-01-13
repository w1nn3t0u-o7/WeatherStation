#include <iostream>

#include "spi.hpp"
#include "i2c.hpp"
#include "bme280.hpp"

constexpr static int SPI_3_MISO = 19;
constexpr static int SPI_3_MOSI = 23;
constexpr static int SPI_3_SCLK = 18;
constexpr static int BME280_SS_PIN = 5;

constexpr static int I2C_SDA = 17;
constexpr static int I2C_SCL = 16;
constexpr static uint32_t I2C_CLK_SPEED_HZ = 400000;

MZDK::I2c i2c {I2C_NUM_0};
MZDK::Spi spi3;
MZDK::BME280 bme280(&spi3);

extern "C" void app_main(void)
{    

    // Initialise the I2C
    i2c.InitMaster(I2C_SDA, I2C_SCL, I2C_CLK_SPEED_HZ, true, true);
    i2c.InitI2cForBme280(0x76);
    // Initialize the SPI
    spi3.Init(SPI3_HOST, SPI_3_MISO, SPI_3_MOSI, SPI_3_SCLK);
    // Register BME280 device as SPI device using GPIO5 as the select pin
    spi3.InitSpiForBme280(BME280_SS_PIN);
    
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


