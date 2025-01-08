#pragma once

#include "soc/soc.h"

namespace MZDK {
class ComProtocol {
public:
    uint8_t *rx_buf{};

    virtual int write(uint8_t addr, uint8_t *data, size_t len) = 0;
    virtual int read(uint8_t addr, uint8_t *data, size_t len) = 0;
    virtual int writeRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t value) = 0;
    virtual int readRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t *value) = 0;
    virtual ~ComProtocol() {}
};

static uint64_t read_sys_timer() {
    uint32_t low = REG_READ(SYS_TIMER_COUNT_LOW);
    uint32_t high = REG_READ(SYS_TIMER_COUNT_HIGH);
    return ((uint64_t)high << 32) | low;
}

static void delay_us(uint32_t us) {
    if (us == 0) return;
    uint64_t start_time = read_sys_timer();
    uint64_t wait_time = us * 80; // Timer działa z zegarem 80 MHz
    while (read_sys_timer() - start_time < wait_time) {
        // Czekamy na upływ czasu
    }
}
}