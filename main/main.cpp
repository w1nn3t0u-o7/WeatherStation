//#include "gpio.hpp"
//#include "i2c.hpp"
#include "spi.hpp"
#include "bme280.cpp"
#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main(void) {
    
    MZDK::GPIO mosi(23, MZDK::GPIO_MODE_OUTPUT, MZDK::GPIO_PULL_NONE);
    MZDK::GPIO miso(19, MZDK::GPIO_MODE_INPUT, MZDK::GPIO_PULL_NONE);
    MZDK::GPIO sclk(18, MZDK::GPIO_MODE_OUTPUT, MZDK::GPIO_PULL_NONE);
    MZDK::GPIO cs(5, MZDK::GPIO_MODE_OUTPUT, MZDK::GPIO_PULL_NONE);
    MZDK::SPI spi(2, cs, mosi, miso, sclk);

    MZDK::BME280 bme280(&spi);
    MZDK::BME280::BME280ResultData results;
    bme280.GetAllResults(&results);
    while(true) {
        bme280.GetAllResults(&results);
        std::cout << "==================================================\n";
        std::cout << "SPI Temperature: " << results.temperature << "c\n";
        std::cout << "SPI Humidity   : " << results.humididty << "%\n";
        std::cout << "SPI Pressure   : " << results.pressure << "Pa\n";
        std::cout << "--------------------------------------------------\n";

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}