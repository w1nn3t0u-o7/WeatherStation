#include "soc/i2c_struct.h"
#include "soc/gpio_periph.h"
#include <stdexcept>
#include <cstdio>
#include "gpio.hpp"


class I2CDriver {
private:
    i2c_dev_t* i2c;
    MZDK::GPIO sda;
    MZDK::GPIO scl;

    void reset() {
        i2c->ctr.trans_start = 0;
        i2c->fifo_conf.tx_fifo_rst = 1;
        i2c->fifo_conf.tx_fifo_rst = 0;
        i2c->fifo_conf.rx_fifo_rst = 1;
        i2c->fifo_conf.rx_fifo_rst = 0;
    }

    void start() {
        i2c->command[0].val = 0;
        i2c->command[0].op_code = 0;
        i2c->ctr.trans_start = 1;
    }

    void stop() {
        i2c->command[0].val = 0;
        i2c->command[0].op_code = 3;
        i2c->ctr.trans_start = 1;
    }

    int validateAck() {
        while (i2c->status_reg.bus_busy){

        }

        if (i2c->status_reg.rx_ack_value == 0)
    }

public:
    I2CDriver(i2c_dev_t* i2c_port, int sda_pin, int scl_pin)
        : i2c(i2c_port), sda(sda_pin, MZDK::GPIO_MODE_INPUT_OUTPUT_OD, MZDK::GPIO_PULL_UP), scl(scl_pin, MZDK::GPIO_MODE_INPUT_OUTPUT_OD, MZDK::GPIO_PULL_UP) {
        i2c->ctr.val = 0;
        i2c->ctr.ms_mode = 1;
        i2c->ctr.scl_force_out = 1;
        i2c->ctr.sda_force_out = 1;
        i2c->timeout.tout = 400000;
        reset();
    }

    int write(uint8_t addr, uint8_t* data, size_t len) {
        start();
        i2c->fifo_data.data = (addr << 1) | 0;
        for (size_t i = 0; i < len; ++i) {
            i2c->fifo_data.data = data[i];
        }
        stop();
        return 0;
    }

    int read(uint8_t addr, uint8_t* data, size_t len) {
        start();
        i2c->fifo_data.data = (addr << 1) | 1;
        for (size_t i = 0; i < len; ++i) {
            data[i] = i2c->fifo_data.data;
        }
        stop();
        return 0;
    }
};
