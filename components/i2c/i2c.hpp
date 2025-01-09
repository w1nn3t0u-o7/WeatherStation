#include "soc/i2c_struct.h"
#include "soc/i2c_reg.h"
#include "soc/dport_reg.h"
#include "com_protocols.hpp"
#include <cstring>
#include <iostream>

namespace MZDK {

class I2CMaster : public ComProtocol {
private:
    i2c_dev_t *i2c;
    uint8_t device_address;

    void scanner() {}

    void enable_peripheral() {
        if (i2c == &I2C0) {
            DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_I2C_EXT0_CLK_EN);
            DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_I2C_EXT0_RST);
        } else if (i2c == &I2C1) {
            DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_I2C_EXT1_CLK_EN);
            DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_I2C_EXT1_RST);
        }
    }

    int wait_for_done() {
        // Czekaj na zakończenie transmisji lub timeout
        int timeout = 100000;
        while (!(i2c->int_raw.end_detect || i2c->int_raw.time_out) && --timeout);
        if (timeout <= 0 || i2c->int_raw.time_out) {
            return 1;
        }
        return 0;
    }

public:
    I2CMaster(int i2c_num, uint8_t addr) : device_address(addr) {
        if (i2c_num == 0) {
            i2c = &I2C0;
        } else if (i2c_num == 1) {
            i2c = &I2C1;
        } else {
            i2c = nullptr;
        }
        enable_peripheral();
        init();
    }

    void init() {
        if (!i2c) return;
        // Ustawienie zegara
        i2c->scl_low_period.period = 40;  // Okres niski dla SCL
        i2c->scl_high_period.period = 40; // Okres wysoki dla SCL

        // Ustawienia czasów start/stop
        i2c->scl_start_hold.time = 10; // Opóźnienie startu
        i2c->scl_stop_hold.time = 10;  // Opóźnienie stopu
        i2c->scl_rstart_setup.time = 10;
        i2c->scl_stop_setup.time = 10;

        // Włącz czas nadzoru
        i2c->timeout.tout = 400; // Timeout na 400 cykli

        // Włączenie I2C w trybie master
        i2c->ctr.val = 0;
        i2c->ctr.ms_mode = 1;  // Tryb master
        i2c->ctr.trans_start = 0; // Brak aktywnej transmisji
    }

    int write(uint8_t reg, uint8_t data) override {
        
        // Ustawienie adresu urządzenia w trybie zapisu
        i2c->fifo_data.data = (device_address << 1); // 7-bitowy adres + bit zapisu (0)
        i2c->fifo_data.data = reg;                  // Rejestr docelowy

        // Zapisz dane do FIFO
        for (size_t i = 0; i < len; ++i) {
            i2c->fifo_data.data = data[i];
        }

        // Rozpoczęcie transmisji
        i2c->command[0].op_code = 0x0; // START
        i2c->command[0].done = 0;

        i2c->command[1].op_code = 0x1; // WRITE
        i2c->command[1].byte_num = len + 1; // +1 dla adresu i rejestru

        i2c->command[2].op_code = 0x2; // STOP
        i2c->command[2].done = 0;

        i2c->ctr.trans_start = 1; // Rozpoczęcie transmisji

        // Poczekaj na zakończenie
        int ret = wait_for_done();
        if (ret != 0) return ret;

        return 0;
    }

    int read(uint8_t reg) override {

        // Ustawienie adresu urządzenia w trybie zapisu
        i2c->fifo_data.data = (device_address << 1); // 7-bitowy adres + bit zapisu (0)
        i2c->fifo_data.data = reg;                  // Rejestr docelowy

        // Rozpoczęcie transmisji
        i2c->command[0].op_code = 0x0; // START
        i2c->command[0].done = 0;

        i2c->command[1].op_code = 0x1; // WRITE
        i2c->command[1].byte_num = 2;  // 1 bajt adresu + 1 bajt rejestru

        i2c->command[2].op_code = 0x3; // RESTART
        i2c->command[2].done = 0;

        i2c->command[3].op_code = 0x1; // READ
        i2c->command[3].byte_num = 1;

        i2c->command[4].op_code = 0x2; // STOP
        i2c->command[4].done = 0;

        i2c->ctr.trans_start = 1; // Rozpoczęcie transmisji

        // Poczekaj na zakończenie
        int ret = wait_for_done();
        if (ret != 0) return ret;

        // Odczytaj dane z FIFO
        for (size_t i = 0; i < len; ++i) {
            data[i] = i2c->fifo_data.data;
        }

        return 0;
    }
};


}
