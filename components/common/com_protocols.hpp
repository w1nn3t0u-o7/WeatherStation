#pragma once

#include "soc/soc.h"
#include <cstring>

namespace MZDK {
class ComProtocol {
public:
    uint8_t *rx_buf{};

    virtual uint8_t write(uint8_t reg, uint8_t data) = 0;
    virtual uint8_t read(uint8_t reg) = 0;
    virtual ~ComProtocol() {}
};
}