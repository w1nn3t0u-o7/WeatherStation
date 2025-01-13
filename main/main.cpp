#include "i2c.hpp"
#include "spi.hpp"
#include "gpio.hpp"
#include "bme280.cpp"
#include <iostream>
//#include "driver/gpio.h"


extern "C" void app_main(void) {
    MZDK::GPIO scl(22, MZDK::GPIO_MODE_INPUT_OUTPUT_OD, MZDK::GPIO_PULL_UP);
    MZDK::GPIO sda(21, MZDK::GPIO_MODE_INPUT_OUTPUT_OD, MZDK::GPIO_PULL_UP);
    MZDK::I2C i2c(0);
    i2c.InitMaster(21, 22);

    MZDK::GPIO mosi(23, MZDK::GPIO_MODE_OUTPUT, MZDK::GPIO_PULL_NONE);
    MZDK::GPIO miso(19, MZDK::GPIO_MODE_INPUT, MZDK::GPIO_PULL_NONE);
    //gpio_set_direction(GPIO_NUM_19, GPIO_MODE_INPUT);
    MZDK::GPIO sclk(18, MZDK::GPIO_MODE_OUTPUT, MZDK::GPIO_PULL_NONE);
    MZDK::GPIO cs(5, MZDK::GPIO_MODE_OUTPUT, MZDK::GPIO_PULL_NONE);
    MZDK::SPI spi;
    spi.Init(&SPI2, 19, 23, 18, 5, (1 << 8) | 7);


    MZDK::BME280 bme280(&spi);
    MZDK::BME280::BME280ResultData results;
    while(true) {
        bme280.GetDeviceID();
        bme280.GetAllResults(&results);
        std::cout << "==================================================" << std::endl;
        std::cout << "SPI Temperature: " << results.temperature << "c" << std::endl;
        std::cout << "SPI Humidity   : " << results.humididty << "%" << std::endl;
        std::cout << "SPI Pressure   : " << results.pressure << "Pa" << std::endl;
        std::cout << "--------------------------------------------------" << std::endl;

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


