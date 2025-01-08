#include "soc/spi_reg.h"
#include "gpio.hpp"
#include "com_protocols.hpp"

namespace MZDK {
class SPI : public ComProtocol {
private:
    int m_port;
    GPIO m_mosi;
    GPIO m_miso;
    GPIO m_sclk;
    GPIO m_cs;

    void m_reset();
    int m_validate_transfer();

public:
    SPI(int port, int mosi_pin, int miso_pin, int sclk_pin, int cs_pin);

    int write(uint8_t addr, uint8_t* data, size_t len) override;
    int read(uint8_t addr, uint8_t* data, size_t len) override;
};
}