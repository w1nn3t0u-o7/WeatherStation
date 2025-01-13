#include "i2c.hpp"

void MZDK::I2C::SetClockSpeed(uint32_t clk_speed)
{
    I2C0.scl_low_period.period = clk_speed / 2;
    I2C0.scl_high_period.period = clk_speed / 2;
}

void MZDK::I2C::SetPins(int sda_io_num, int scl_io_num)
{
    _sda_io_num = sda_io_num;
    _scl_io_num = scl_io_num;

    // Configure SDA pin
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[sda_io_num], PIN_FUNC_GPIO);
    // gpio_set_direction((gpio_num_t)sda_io_num, GPIO_MODE_INPUT_OUTPUT);

    // Configure SCL pin
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[scl_io_num], PIN_FUNC_GPIO);
    // gpio_set_direction((gpio_num_t)scl_io_num, GPIO_MODE_INPUT_OUTPUT);
}

void MZDK::I2C::StartCondition()
{
    I2C0.command[0].op_code = 0;
}

void MZDK::I2C::StopCondition()
{
    I2C0.command[0].op_code = 3;
}

void MZDK::I2C::WaitForCompletion()
{
    while (I2C0.status_reg.bus_busy);
}

MZDK::I2C::I2C(int port, uint32_t clk_speed) : _port(port), _clk_speed(clk_speed) {}

int MZDK::I2C::InitMaster(int sda_io_num, int scl_io_num)
{
    if (_port != 0)
    {
        return 1; // Currently supporting only I2C0
    }

    SetPins(sda_io_num, scl_io_num);
    SetClockSpeed(_clk_speed);

    // Enable I2C peripheral
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_I2C_EXT0_CLK_EN);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_I2C_EXT0_RST);

    return 0;
}

uint8_t MZDK::I2C::readRegister(uint8_t reg_addr)
{
    uint8_t rxBuf = 0;

    StartCondition();

    // Send device address (write mode)
    I2C0.fifo_data.data = (dev_addr << 1) | 0x0;
    WaitForCompletion();

    // Send register address
    I2C0.fifo_data.data = reg_addr;
    WaitForCompletion();

    StartCondition();

    // Send device address (read mode)
    I2C0.fifo_data.data = (dev_addr << 1) | 0x1;
    WaitForCompletion();

    // Read data
    rxBuf = I2C0.fifo_data.data;

    StopCondition();
    WaitForCompletion();

    return rxBuf;
}

uint16_t MZDK::I2C::readWord(const uint8_t reg_addr) {
    return ((readRegister(reg_addr) << 8) | readRegister(reg_addr + 1));
}

int MZDK::I2C::writeRegister(const uint8_t reg_addr,const uint8_t data)
{
    StartCondition();

    // Send device address (write mode)
    I2C0.fifo_data.data = (dev_addr << 1) | 0x0;
    WaitForCompletion();

    // Send register address
    I2C0.fifo_data.data = reg_addr;
    WaitForCompletion();

    // Send data
    I2C0.fifo_data.data = data;
    WaitForCompletion();

    StopCondition();
    WaitForCompletion();

    return 0;
}

