#include "i2c.hpp"

namespace MZDK {
    I2C::I2C(i2c_port_t port, i2c_mode_t mode, int sda_pin, int scl_pin, bool sda_pullup_en, bool scl_pullup_en) : m_port(port), m_mode(mode) {

        _config.mode = mode;
        _config.sda_io_num = sda_pin;
        _config.scl_io_num = scl_pin;
        _config.master.clk_speed = I2C_CLK_SPEED_HZ;
        _config.sda_pullup_en = sda_pullup_en;
        _config.scl_pullup_en = scl_pullup_en;
        _config.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;

        i2c_param_config(m_port, &_config);
        
        i2c_driver_install(m_port, m_mode, 0, 0, 0);
    }

    I2C::~I2C() {
        i2c_driver_delete(m_port);
    }

    uint8_t I2C::readRegister(uint8_t reg_addr) {
        uint8_t rx_buf{};

        i2c_master_write_read_device(m_port, m_dev_addr, &reg_addr, 1, &rx_buf, 1, pdMS_TO_TICKS(1000));

        return rx_buf;
    }

    int I2C::writeRegister(uint8_t reg_addr, uint8_t tx_data) {
        const uint8_t tx_buf[2] {reg_addr, tx_data};
        return i2c_master_write_to_device(m_port, m_dev_addr, tx_buf, 2, pdMS_TO_TICKS(1000));
    }

    int I2C::readRegisterMultipleBytes(uint8_t reg_addr, uint8_t *rx_data, int length) {
        return i2c_master_write_read_device(m_port, m_dev_addr, &reg_addr, 1, rx_data, length, pdMS_TO_TICKS(1000));
    }

    int I2C::writeRegisterMultipleBytes(uint8_t reg_addr, uint8_t *tx_data, int length) {
        int status = 0;
        uint8_t buffer[I2C_LINK_RECOMMENDED_SIZE(3)] = { 0 };

        i2c_cmd_handle_t _handle = i2c_cmd_link_create_static(buffer, sizeof(buffer));
        status |= i2c_master_start(_handle);
        status |= i2c_master_write_byte(_handle, (m_dev_addr << 1) | I2C_MASTER_WRITE, true);
        status |= i2c_master_write_byte(_handle, reg_addr, true);
        status |= i2c_master_write(_handle, tx_data, length, true);
        status |= i2c_master_stop(_handle);
        status |= i2c_master_cmd_begin(m_port, _handle, pdMS_TO_TICKS(1000));
        i2c_cmd_link_delete_static(_handle);

        return status;
    }

    int I2C::read2Registers(const uint8_t reg) {
        uint8_t buff[2];
        readRegisterMultipleBytes(reg, buff, 2);
        return buff[1] << 8 | buff[0];
    }

    void I2C::initI2cForBme280(const uint8_t dev_addr) {
        m_dev_addr = dev_addr;
    }
} 
