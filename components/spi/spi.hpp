#pragma once

#include "soc/spi_struct.h"
#include "soc/spi_reg.h"
#include "soc/dport_reg.h"
#include <cstring>

#include "com_protocols.hpp"
#include "gpio.hpp"

namespace MZDK {

class SPI : public ComProtocol {
private:
    spi_dev_t *m_spi;
    GPIO m_cs, m_mosi, m_miso, m_sclk;

    void m_enablePeripheral() override;

public:
    SPI(int spi_num, GPIO cs, GPIO mosi, GPIO miso, GPIO sclk);

    void init();
    uint8_t write(uint8_t reg, uint8_t data) override;

    uint8_t read(uint8_t reg) override;

    int readWord(uint8_t reg) override;
};

}
