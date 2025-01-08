#include "soc/i2c_reg.h"
#include "gpio.hpp"
#include "com_protocols.hpp"

namespace MZDK {
    enum I2CMasterDirection {I2C_MASTER_WRITE, I2C_MASTER_READ};
class I2C : public ComProtocol {
private:
    int m_port;
    GPIO m_sda;
    GPIO m_scl;

    void m_reset();
    void m_start();
    void m_stop();

    int m_validateAck();

public:
    I2C(int port = 0, int sda_pin, int scl_pin);

    uint8_t rx_buf{};

    int write(uint8_t addr, uint8_t* data, size_t len) override;
    int read(uint8_t addr, uint8_t *data, size_t len) override;
    int writeRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t value) override;
    int readRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t *value) override;
};
}
