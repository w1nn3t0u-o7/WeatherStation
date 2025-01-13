#pragma once

#include "soc/soc.h"
#include <cstring>

namespace MZDK {
class ComProtocol {
private:
    //virtual void m_enablePeripheral() = 0;
public:

    virtual uint8_t readRegister(const uint8_t reg_addr) = 0;
    virtual uint16_t readWord(const uint8_t reg_addr) = 0;
    virtual int writeRegister(const uint8_t reg_addr, const uint8_t reg_data) = 0;
    virtual ~ComProtocol() {}
};
}