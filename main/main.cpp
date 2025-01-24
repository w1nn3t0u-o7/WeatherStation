#include <iostream>

#include "spi.hpp"
#include "i2c.hpp"
#include "bme280.hpp"
#include "wifi.hpp"

constexpr static int BME280_CS_PIN = 5;

extern "C" void app_main(void) {
    MZDK::WIFI::state wifi_state { MZDK::WIFI::state::NOT_INITIALIZED };
    MZDK::WIFI wifi;
    wifi.setCredentials("1234", "12345678");
    wifi.init();

    float Temperature{};
    float Pressure{};
    int Humidity{};
    int Id{};

    MZDK::I2C i2c(I2C_NUM_0, I2C_MODE_MASTER, 21, 22, true, true);
    i2c.initI2cForBme280(0x76);

    MZDK::SPI spi(SPI3_HOST);
    spi.initSpiForBme280(BME280_CS_PIN);

    MZDK::BME280 bme280(&i2c);

    while(true) {
        Id = bme280.getDeviceID();
        bme280.getAllResults(&Temperature, &Humidity, &Pressure);
        std::cout << "==================================================" << std::endl;
        std::cout << "Temperature: " << Temperature << "c" << std::endl;
        std::cout << "Humidity   : " << Humidity << "%" << std::endl;
        std::cout << "Pressure   : " << Pressure << "Pa" << std::endl;
        std::cout << "ID         : " << Id << std::endl;
        std::cout << "==================================================" << std::endl;

        wifi_state = wifi.getState();

        switch (wifi_state) {
        case MZDK::WIFI::state::READY_TO_CONNECT:
            std::cout << "Wifi Status: READY_TO_CONNECT\n";
            wifi.begin();
            break;
        case MZDK::WIFI::state::DISCONNECTED:
            std::cout << "Wifi Status: DISCONNECTED\n";
            wifi.begin();
            break;
        case MZDK::WIFI::state::CONNECTING:
            std::cout << "Wifi Status: CONNECTING\n";
            break;
        case MZDK::WIFI::state::WAITING_FOR_IP:
            std::cout << "Wifi Status: WAITING_FOR_IP\n";
            break;
        case MZDK::WIFI::state::ERROR:
            std::cout << "Wifi Status: ERROR\n";
            break;
        case MZDK::WIFI::state::CONNECTED:
            std::cout << "Wifi Status: CONNECTED\n";
            break;
        case MZDK::WIFI::state::NOT_INITIALIZED:
            std::cout << "Wifi Status: NOT_INITIALIZED\n";
            break;
        case MZDK::WIFI::state::INITIALIZED:
            std::cout << "Wifi Status: INITIALIZED\n";
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


