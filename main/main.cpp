#include "gpio.hpp"
#include "i2c.hpp"
#include "spi.hpp"
#include "bme280.cpp"

extern "C" void app_main(void) {
    GPIO led(2, GPIO::Direction::OUTPUT);
    led.write(true);

    GPIO sda(21, GPIO::Direction::OUTPUT);
    GPIO scl(22, GPIO::Direction::OUTPUT);
    I2C i2c(sda, scl);
    
    GPIO mosi(23, GPIO::Direction::OUTPUT);
    GPIO miso(19, GPIO::Direction::INPUT);
    GPIO sclk(18, GPIO::Direction::OUTPUT);
    GPIO cs(5, GPIO::Direction::OUTPUT);
    SPI spi(mosi, miso, sclk, cs);
}