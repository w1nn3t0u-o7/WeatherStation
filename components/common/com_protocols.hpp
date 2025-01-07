#pragma once

#include "registers.hpp"

class ComProtocol {
public:
    virtual void write_byte(uint8_t byte) = 0;
    virtual uint8_t read_byte() = 0;
    virtual ~ComProtocol() {}
};

static uint64_t read_sys_timer() {
    uint32_t low = *reg(SYS_TIMER_COUNT_LOW);
    uint32_t high = *reg(SYS_TIMER_COUNT_HIGH);
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