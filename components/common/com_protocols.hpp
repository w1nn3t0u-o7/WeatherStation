#pragma once

#include "soc/soc.h"
#include <cstring>

namespace MZDK {
class ComProtocol {
private:
    virtual void m_enablePeripheral() = 0;
public:

    virtual uint8_t write(uint8_t reg, uint8_t data) = 0;
    virtual uint8_t read(uint8_t reg) = 0;
    virtual int readWord(uint8_t reg) = 0;
    virtual ~ComProtocol() {}
};
}