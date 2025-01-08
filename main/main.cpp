#include "gpio.hpp"
#include "i2c.hpp"
#include "spi.hpp"
#include "bme280.cpp"

extern "C" void app_main(void) {
    MZDK::GPIO led(2, MZDK::GPIO::Direction::OUTPUT);
    led.write(MZDK::GPIO::State::HIGH);

    MZDK::GPIO sda(21, MZDK::GPIO::Direction::OUTPUT);
    MZDK::GPIO scl(22, MZDK::GPIO::Direction::OUTPUT);
    MZDK::I2C i2c(sda, scl);
    
    MZDK::GPIO mosi(23, MZDK::GPIO::Direction::OUTPUT);
    MZDK::GPIO miso(19, MZDK::GPIO::Direction::INPUT);
    MZDK::GPIO sclk(18, MZDK::GPIO::Direction::OUTPUT);
    MZDK::GPIO cs(5, MZDK::GPIO::Direction::OUTPUT);
    MZDK::SPI spi(mosi, miso, sclk, cs);
}