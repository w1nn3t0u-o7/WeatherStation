#include "i2c.hpp"

void MZDK::I2C::m_reset() {
    REG_WRITE(I2C_FIFO_CONF_REG(m_port), I2C_TX_FIFO_RST | I2C_RX_FIFO_RST);
    REG_WRITE(I2C_FIFO_CONF_REG(m_port), 0);
}

void MZDK::I2C::m_start() {
    REG_WRITE(I2C_COMD0_REG(m_port), (0x0 << 11));
}

void MZDK::I2C::m_stop() {
    REG_WRITE(I2C_COMD0_REG(m_port), (0x3 << 11));
}

int MZDK::I2C::m_validateAck() {
    while (REG_READ(I2C_SR_REG(m_port)) & I2C_BUS_BUSY) {
        // czekaj aż magistrala będzie wolna
    }

    if ((REG_READ(I2C_SR_REG(m_port)) & I2C_ACK_REC) == 0) {
        return 0; // ACK received
    } else {
        return 1; // NACK received
    }
}

MZDK::I2C::I2C(int port = 0, int sda_pin, int scl_pin)
            : m_port(port), m_sda(sda_pin, GPIO_MODE_INPUT_OUTPUT_OD, GPIO_PULL_UP), m_scl(scl_pin, GPIO_MODE_INPUT_OUTPUT_OD, GPIO_PULL_UP) {
    m_reset();
}

int MZDK::I2C::write(uint8_t addr, uint8_t* data, size_t len) {
    m_start();
    REG_WRITE(I2C_DATA_REG(m_port), (addr << 1) | I2C_MASTER_WRITE);
    if (m_validateAck() != 0) {
        m_stop();
        return 1;
    }

    for (size_t i = 0; i < len; ++i) {
        REG_WRITE(I2C_DATA_REG(m_port), data[i]);
        if (m_validateAck() != 0 ) {
            m_stop();
            return 1;
        }
    }
    m_stop();
    return 1;
}

int MZDK::I2C::read(uint8_t addr, uint8_t *data, size_t len) {
    m_start();
    REG_WRITE(I2C_DATA_REG(m_port), (addr << 1) | I2C_MASTER_READ);
    if (m_validateAck() != 0) {
        m_stop();
        return 1;
    }

    for (size_t i = 0; i < len; ++i) {
        data[i] = REG_READ(I2C_DATA_REG(m_port));
        if (m_validateAck() != 0) {
            m_stop();
            return 1;
        }
    }
    m_stop();
    return 0;
}