#pragma once

namespace MZDK {
    class ComProtocol {
    public:
        virtual uint8_t readRegister(const uint8_t reg_addr) = 0;
        virtual int writeRegister(const uint8_t reg_addr, const uint8_t tx_data) = 0;
        virtual int readRegisterMultipleBytes(const uint8_t reg_addr, uint8_t *rx_data, const int length) = 0;
        virtual int writeRegisterMultipleBytes(const uint8_t reg_addr, uint8_t *tx_data, const int length) = 0;
    
        virtual int read2Registers(const uint8_t reg_addr) = 0;

        virtual ~ComProtocol() {}
    };
}