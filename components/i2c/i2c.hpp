// WIP
#pragma once

#include "soc/i2c_struct.h"
#include "soc/i2c_reg.h"
#include "soc/dport_reg.h"
#include "com_protocols.hpp"
#include "gpio.hpp"
#include <cstring>
#include <iostream>

namespace MZDK {

class I2C : public ComProtocol {
private:
    i2c_dev_t *m_i2c;
    uint8_t m_device_address;

    void m_scanner();
    void m_enablePeripheral() override;
    int m_waitForDone();

public:
    I2C(int port, uint8_t addr);

    void init();

    int write(uint8_t reg, uint8_t data) override;

    int read(uint8_t reg) override;
};

}
