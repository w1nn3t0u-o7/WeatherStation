#pragma once

#include "soc/i2c_struct.h"
#include "soc/i2c_reg.h"
#include "soc/dport_reg.h"
//#include "driver/gpio.h"
#include "gpio.hpp"
#include "com_protocols.hpp"

namespace MZDK
{
    class I2C : public ComProtocol {
    private:
        uint8_t dev_addr = 0x77;
        uint16_t _slaveAddr{};
        int _port{};
        int _sda_io_num{};
        int _scl_io_num{};
        uint32_t _clk_speed{};

        void SetClockSpeed(uint32_t clk_speed);

        void SetPins(int sda_io_num, int scl_io_num);

        void StartCondition();

        void StopCondition();

        void WaitForCompletion();

    public:
        I2C(int port, uint32_t clk_speed = 100000);

        int InitMaster(int sda_io_num, int scl_io_num);

        uint8_t readRegister(const uint8_t reg_addr) override;

        uint16_t readWord(const uint8_t reg_addr) override;

        int writeRegister(const uint8_t reg_addr,const uint8_t reg_data) override;
    };
} // namespace CPPI2C


