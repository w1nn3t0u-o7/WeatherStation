#pragma once

#include <cstring>
#include "esp_err.h"

namespace MZDK {
class ComProtocol {
private:
    
public:

    virtual esp_err_t writeByteData(const uint8_t reg, const uint8_t value) = 0;
    virtual int readByteData(const uint8_t reg) = 0;
    virtual int readWordData(const uint8_t reg) = 0;
    virtual esp_err_t readBlockData(const uint8_t reg, uint8_t *buf, const int length) = 0;
    virtual ~ComProtocol() {}
};
}