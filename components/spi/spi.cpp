#include "spi.hpp"

void MZDK::SPI::configurePins()
{
    // Configure MISO, MOSI, SCLK, and CS pins as GPIOs with appropriate modes
    GPIO.func_out_sel_cfg[_pin_mosi].func_sel = 2;
    GPIO.func_out_sel_cfg[_pin_sclk].func_sel = 0;
    GPIO.func_in_sel_cfg[_pin_miso].func_sel = 2;
    GPIO.func_out_sel_cfg[_pin_cs].func_sel = 5;

    // Set CS pin as output
    //GPIO.enable_w1ts = (1 << _pin_cs);
    GPIO.out_w1ts = (1 << _pin_cs); // Set CS high
}

void MZDK::SPI::resetSpi()
{
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI2_CLK_EN);
    // mode0
    _spi_dev->pin.ck_idle_edge = 1;
    _spi_dev->user.ck_i_edge = 0;
    _spi_dev->ctrl2.miso_delay_mode = 0;
    _spi_dev->ctrl2.miso_delay_num = 0;
    _spi_dev->ctrl2.mosi_delay_mode = 2;
    _spi_dev->ctrl2.mosi_delay_num = 2;

    // Reset SPI peripheral to a known state
    // _spi_dev->slave.trans_done = 0;
    _spi_dev->clock.clk_equ_sysclk = 0; // Disable system clock equivalence
    _spi_dev->clock.clkdiv_pre = (_clock_div >> 8) & 0x1F; // Pre-divider (5 bits)
    _spi_dev->clock.clkcnt_n = _clock_div & 0xFF;          // Clock divider (8 bits)
    _spi_dev->clock.clkcnt_h = (_spi_dev->clock.clkcnt_n >> 1); // Half clock cycle
    _spi_dev->clock.clkcnt_l = _spi_dev->clock.clkcnt_n;       // Full clock cycle

    //_spi_dev->user.val = 0; // Clear user configuration
    _spi_dev->pin.val = 0;  // Clear pin configuration

    _spi_dev->user.doutdin = 1;    // Full-duplex
}


int MZDK::SPI::Init(spi_dev_t* spi_dev, const int pin_miso, const int pin_mosi, const int pin_sclk, const int pin_cs, uint32_t clock_div)
{
    _spi_dev = spi_dev;
    _pin_miso = pin_miso;
    _pin_mosi = pin_mosi;
    _pin_sclk = pin_sclk;
    _pin_cs = pin_cs;
    _clock_div = clock_div;

    configurePins();
    resetSpi();

    return 0;
}

int MZDK::SPI::transferByte(const uint8_t reg_addr, const uint8_t data, uint8_t& rx_data, const uint8_t command)
{
    // Begin transaction
    GPIO.out_w1tc = (1 << _pin_cs); // CS low

    // Send command and address
    _spi_dev->data_buf[0] = (command << 8) | reg_addr;
    _spi_dev->cmd.usr = 1;
    while (_spi_dev->cmd.usr);

    // Send data
    _spi_dev->data_buf[0] = data;
    _spi_dev->cmd.usr = 1;
    while (_spi_dev->cmd.usr);

    // Read data
    rx_data = _spi_dev->data_buf[0] & 0xFF;

    // End transaction
    GPIO.out_w1ts = (1 << _pin_cs); // CS high

    return 0;
}

int MZDK::SPI::readByte(const uint8_t reg_addr) {
    uint8_t rx_data;
    // Begin transaction
    GPIO.out_w1tc = (1 << _pin_cs); // CS low

    _spi_dev->user.usr_command = 1;
    _spi_dev->user2.usr_command_bitlen = 1;
    _spi_dev->user2.usr_command_value = 0;
    _spi_dev->cmd.usr = 1;
    while (_spi_dev->cmd.usr) {
        ;
    }
    _spi_dev->user.val = 0;
    _spi_dev->user.usr_addr = 1;
    _spi_dev->user1.usr_addr_bitlen = 7;
    _spi_dev->data_buf[0] = reg_addr;
    _spi_dev->cmd.usr = 1;
    while (_spi_dev->cmd.usr) {
        ;
    }
    _spi_dev->user.val = 0;
    _spi_dev->user.usr_miso = 1;
    _spi_dev->miso_dlen.usr_miso_dbitlen = 7;
    _spi_dev->cmd.usr = 1;
    while (_spi_dev->cmd.usr) {
        ;
    }

    // End transaction
    GPIO.out_w1ts = (1 << _pin_cs); // CS high
    rx_data = _spi_dev->data_buf[0];
    return rx_data;
}

int MZDK::SPI::writeByte(const uint8_t reg_addr, const uint8_t data) {
    // Begin transaction
    GPIO.out_w1tc = (1 << _pin_cs); // CS low
    _spi_dev->user.usr_command = 1;
    _spi_dev->user2.usr_command_bitlen = 1;
    _spi_dev->user2.usr_command_value = 0;
    _spi_dev->cmd.usr = 1;
    while (_spi_dev->cmd.usr) {
        ;
    }
    _spi_dev->user.val = 0;
    _spi_dev->user.usr_addr = 1;
    _spi_dev->user1.usr_addr_bitlen = 7;
    _spi_dev->data_buf[0] = reg_addr;
    _spi_dev->cmd.usr = 1;
    while (_spi_dev->cmd.usr) {
        ;
    }
    _spi_dev->user.val = 0;
    _spi_dev->user.usr_mosi = 1;
    _spi_dev->mosi_dlen.usr_mosi_dbitlen = 7;
    _spi_dev->data_buf[0] = data;
    _spi_dev->cmd.usr = 1;
    while (_spi_dev->cmd.usr) {
        ;
    }
    _spi_dev->user.val = 0;
    // End transaction
    GPIO.out_w1ts = (1 << _pin_cs); // CS high
    return 0;
}

uint8_t MZDK::SPI::readRegister(const uint8_t reg_addr)
{
    return readByte(reg_addr);
}

uint16_t MZDK::SPI::readWord(const uint8_t reg_addr)
{
    return ((readByte(reg_addr) << 8) | readByte(reg_addr + 1));
}

int MZDK::SPI::writeRegister(const uint8_t reg_addr, const uint8_t reg_data)
{
    return writeByte(reg_addr, reg_data);
}



