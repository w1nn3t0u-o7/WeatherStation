#include <iostream>

#include "spi.hpp"
#include "i2c.hpp"
#include "bme280_spi.hpp"
#include "bme280_i2c.hpp"

constexpr static int SPI_3_MISO = 19;
constexpr static int SPI_3_MOSI = 23;
constexpr static int SPI_3_SCLK = 18;
constexpr static int BME280_SS_PIN = 5;

constexpr static int I2C_SDA = 17;
constexpr static int I2C_SCL = 16;
constexpr static uint32_t I2C_CLK_SPEED_HZ = 400000;

MZDK::I2c i2c {I2C_NUM_0};
MZDK::Spi spi3;

extern "C" void app_main(void)
{    
    MZDK::BME280SPI bme280spi;
    MZDK::BME280I2C bme280i2c;

    // Initialise the I2C
    i2c.InitMaster(I2C_SDA, I2C_SCL, I2C_CLK_SPEED_HZ, true, true);

    // Initialize the SPI
    spi3.Init(SPI3_HOST, SPI_3_MISO, SPI_3_MOSI, SPI_3_SCLK);

    // Register BME280 device as SPI device using GPIO5 as the select pin
    bme280spi.InitSpi(&spi3, BME280_SS_PIN);
    // Initialize the BME280SPI device
    bme280spi.Init();
    bme280spi.SetMode(1);
    bme280spi.SetConfigFilter(1);

    // Initialize the BME280I2C device
    bme280i2c.InitI2c(&i2c, 0x76);
    bme280i2c.Init();
    bme280i2c.SetMode(1);
    bme280i2c.SetConfigFilter(1);

    float spiTemperature{};
    float spiPressure{};
    int spiHumidity{};
    int spiId{};

    float i2cTemperature{};
    float i2cPressure{};
    int i2cHumidity{};
    int i2cId{};

    while(true)
    {
        spiId = bme280spi.GetDeviceID();
        i2cId = bme280i2c.GetDeviceID();
        bme280spi.GetAllResults(&spiTemperature, &spiHumidity, &spiPressure);
        bme280i2c.GetAllResults(&i2cTemperature, &i2cHumidity, &i2cPressure);
        std::cout << "==================================================\n";
        std::cout << "SPI Temperature: " << spiTemperature << "c\n";
        std::cout << "SPI Humidity   : " << spiHumidity << "%\n";
        std::cout << "SPI Pressure   : " << spiPressure << "Pa\n";
        std::cout << "SPI ID         : " << spiId << '\n';

        std::cout << "--------------------------------------------------\n";

        std::cout << "I2C Temperature: " << i2cTemperature << "c\n";
        std::cout << "I2C Humidity   : " << i2cHumidity << "%\n";
        std::cout << "I2C Pressure   : " << i2cPressure << "Pa\n";
        std::cout << "I2C ID         : " << i2cId << '\n';
        std::cout << "==================================================\n";

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
