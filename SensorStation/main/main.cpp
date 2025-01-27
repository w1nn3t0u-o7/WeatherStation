#include "main.hpp"

extern "C" void app_main(void) {
    MZDK::WIFI wifi;
    wifi.setCredentials("MidgetSpinner", "MiKi0987");
    wifi.init();

    MZDK::SNTP sntp;

    float Temperature{};
    float Pressure{};
    int Humidity{};
    int Id{};

    MZDK::I2C i2c(I2C_NUM_0, I2C_MODE_MASTER, 21, 22, true, true);
    i2c.initI2cForBme280(0x76);

    MZDK::SPI spi(SPI3_HOST);
    spi.initSpiForBme280();

    MZDK::BME280 bme280(&i2c);

    while (wifi.getState() != MZDK::WIFI::state::CONNECTED) {
        MZDK::connectToWifi(&wifi, &sntp);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    while(true) {
        Id = bme280.getDeviceID();
        bme280.getAllResults(&Temperature, &Humidity, &Pressure);
        tcp_client(&Temperature, &Humidity, &Pressure);
        std::cout << "==================================================" << std::endl;
        std::cout << "Temperature: " << Temperature << "c" << std::endl;
        std::cout << "Humidity   : " << Humidity << "%" << std::endl;
        std::cout << "Pressure   : " << Pressure << "Pa" << std::endl;
        std::cout << "ID         : " << Id << std::endl;
        std::cout << "==================================================" << std::endl;

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}


