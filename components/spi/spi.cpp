#include "spi.hpp"

void MZDK::SPI::m_enablePeripheral() {
    if (m_spi == &SPI2) {
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI2_CLK_EN);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI2_RST);
    } else if (m_spi == &SPI3) {
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI3_CLK_EN);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI3_RST);
    }
}

MZDK::SPI::SPI(int port, GPIO cs, GPIO mosi, GPIO miso, GPIO sclk) : m_cs(cs), m_mosi(mosi), m_miso(miso), m_sclk(sclk) {
    if (port == 2) {
        m_spi = &SPI2;
    } else if (port == 3) {
        m_spi = &SPI3;
    }
    m_enablePeripheral();
    init();
}

void MZDK::SPI::init() {
    if (!m_spi) return;

    // Konfiguracja zegara SPI
    m_spi->clock.val = 0; // Domyślne ustawienie zegara
    m_spi->user.val = 0;
    m_spi->user1.val = 0;
    m_spi->ctrl.val = 0;

    // Ustawienie trybu Master
    m_spi->slave.val = 0;
    m_spi->user.usr_miso_highpart = 0;
    m_spi->user.usr_mosi_highpart = 0;

    // Konfiguracja CS
    m_spi->pin.cs0_dis = 0; // Aktywuj CS0
}

uint8_t MZDK::SPI::write(uint8_t reg, uint8_t data) {

    // Przygotowanie bufora danych
    m_spi->data_buf[0] = reg & 0x7F;
    m_spi->data_buf[1] = data;
    // Załaduj dane do sprzętowego bufora transmisji
    

    // Ustaw długość transmisji
    m_spi->mosi_dlen.usr_mosi_dbitlen = 15;

    // Rozpocznij transmisję
    m_spi->cmd.usr = 1;

    // Poczekaj na zakończenie transmisji
    while (m_spi->cmd.usr);

    return 0;
}

uint8_t MZDK::SPI::read(uint8_t reg) {
    uint8_t data;
    // Przygotowanie bufora z adresem rejestru
    m_spi->data_buf[0] = reg | 0x80;

    // Ustaw długość transmisji (1 bajt wysyłany, `len` bajtów odbieranych)
    m_spi->mosi_dlen.usr_mosi_dbitlen = 7;         // 1 bajt = 8 bitów, -1 dla rejestru
    m_spi->miso_dlen.usr_miso_dbitlen = 7;

    // Rozpocznij transmisję
    m_spi->cmd.usr = 1;

    // Poczekaj na zakończenie transmisji
    while (m_spi->cmd.usr);

    // Odczytaj dane z bufora sprzętowego
    data = m_spi->data_buf[0];
    return data;
}

int MZDK::SPI::readWord(uint8_t reg) {
    uint16_t data;
    data = read(reg) << 8 | read(reg+1);
    return data;
}


