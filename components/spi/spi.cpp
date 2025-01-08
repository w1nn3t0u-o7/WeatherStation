#include "spi.hpp"

void MZDK::SPI::m_reset() {
    REG_WRITE(SPI_CMD_REG(m_port), 0);
    REG_WRITE(SPI_SLAVE_REG(m_port), 0);
}

int MZDK::SPI::m_validate_transfer() {
    while (!(REG_READ(SPI_SLAVE_REG(m_port)) & SPI_TRANS_DONE)) {
        // czekaj na zakończenie transmisji
    }
    return 0;
}

MZDK::SPI::SPI(int port, int mosi_pin, int miso_pin, int sclk_pin, int cs_pin)
            : m_port(port), m_mosi(mosi_pin, GPIO_MODE_OUTPUT, GPIO_PULL_NONE),
            m_miso(miso_pin, GPIO_MODE_INPUT, GPIO_PULL_NONE),
            m_sclk(sclk_pin, GPIO_MODE_OUTPUT, GPIO_PULL_NONE),
            m_cs(cs_pin, GPIO_MODE_OUTPUT, GPIO_PULL_NONE) {
    m_reset();
}

int MZDK::SPI::write(uint8_t addr, uint8_t* data, size_t len) {
    m_cs.setLow();
    for (size_t i = 0; i < len; ++i) {
        REG_WRITE(SPI_W0_REG(m_port) + (i * 4), data[i]);
    }
    REG_WRITE(SPI_CMD_REG(m_port), SPI_USR);
    int ret = m_validate_transfer();
    m_cs.setHigh();
    return ret;
}

int MZDK::SPI::read(uint8_t addr, uint8_t* data, size_t len) {
    m_cs.setLow();
    REG_WRITE(SPI_W0_REG(m_port), addr);
    REG_WRITE(SPI_CMD_REG(m_port), SPI_USR);
    int ret = m_validate_transfer();
    if (ret != 0) {
        m_cs.setHigh();
        return ret;
    }
    for (size_t i = 0; i < len; ++i) {
        data[i] = REG_READ(SPI_W0_REG(m_port) + (i * 4));
    }
    m_cs.setHigh();
    return 0;
}


