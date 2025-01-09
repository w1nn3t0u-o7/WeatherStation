#include "soc/spi_struct.h"
#include "soc/spi_reg.h"
#include "soc/dport_reg.h"
#include "com_protocols.hpp"

#include <cstring>

namespace MZDK {
class SPIMaster : public ComProtocol {
private:
    spi_dev_t *spi;
    uint8_t cs_pin;

    void enable_peripheral() {
        if (spi == &SPI2) {
            DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI2_CLK_EN);
            DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI2_RST);
        } else if (spi == &SPI3) {
            DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI3_CLK_EN);
            DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI3_RST);
        }
    }

public:
    SPIMaster(int spi_num, uint8_t cs) : cs_pin(cs) {
        if (spi_num == 2) {
            spi = &SPI2;
        } else if (spi_num == 3) {
            spi = &SPI3;
        } else {
            // W SPI1 w ESP32 jest używany tylko jako slave.
            spi = nullptr;
        }
        enable_peripheral();
        init();
    }

    void init() {
        if (!spi) return;

        // Konfiguracja zegara SPI
        spi->clock.val = 0; // Domyślne ustawienie zegara
        spi->user.val = 0;
        spi->user1.val = 0;
        spi->ctrl.val = 0;

        // Ustawienie trybu Master
        spi->slave.val = 0;
        spi->user.usr_miso_highpart = 0;
        spi->user.usr_mosi_highpart = 0;

        // Konfiguracja CS
        spi->pin.cs0_dis = 0; // Aktywuj CS0
    }

    uint8_t write(uint8_t reg, uint8_t data) override {

        // Przygotowanie bufora danych
        spi->data_buf[0] = reg & 0x7F;
        spi->data_buf[1] = data;
        // Załaduj dane do sprzętowego bufora transmisji
        

        // Ustaw długość transmisji
        spi->mosi_dlen.usr_mosi_dbitlen = 15;

        // Rozpocznij transmisję
        spi->cmd.usr = 1;

        // Poczekaj na zakończenie transmisji
        while (spi->cmd.usr);

        return 0;
    }

    uint8_t read(uint8_t reg) override {
        uint8_t data;
        // Przygotowanie bufora z adresem rejestru
        spi->data_buf[0] = reg | 0x80;

        // Ustaw długość transmisji (1 bajt wysyłany, `len` bajtów odbieranych)
        spi->mosi_dlen.usr_mosi_dbitlen = 7;         // 1 bajt = 8 bitów, -1 dla rejestru
        spi->miso_dlen.usr_miso_dbitlen = 7;

        // Rozpocznij transmisję
        spi->cmd.usr = 1;

        // Poczekaj na zakończenie transmisji
        while (spi->cmd.usr);

        // Odczytaj dane z bufora sprzętowego
        data = spi->data_buf[0];
        return data;
    }
};
}
